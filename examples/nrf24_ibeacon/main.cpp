#include <string.h>
#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <exti.h>
#include <io.h>
#include <spi.h>
#include <drivers/nrf24.h>

#include <board.h>

static constexpr uint8_t rx_addr[5] = {0x00, 0xf0, 0xf0, 0xf0, 0xf0};

typedef SYSCLK_T<> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

#ifdef BOARD_ID_olimexino_stm32
typedef GPIO_T<PF, 0, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> NRF24_IRQ;
typedef GPIO_OUTPUT_T<PF, 1> NRF24_CE;
typedef GPIO_OUTPUT_T<PA, 4> NRF24_CSN;
#elif (defined BOARD_ID_lqfp48_usb_s)
#ifdef STM32F1xx
typedef GPIO_T<PB, 1, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> NRF24_IRQ;
#else
typedef GPIO_T<PB, 1, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> NRF24_IRQ;
#endif
typedef GPIO_OUTPUT_T<PA, 8> NRF24_CE;
typedef GPIO_OUTPUT_T<PA, 4> NRF24_CSN;
#elif (defined BOARD_ID_stm32_e407)
typedef GPIO_T<PE, 6, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> NRF24_IRQ;
typedef GPIO_OUTPUT_T<PG, 7> NRF24_CE;
typedef GPIO_OUTPUT_T<PA, 4> NRF24_CSN;
#endif

typedef EXTI_T<NRF24_IRQ, BUTTON> EXT_INTERRUPT;
typedef SPI_T<SYSCLK, SPI_1, true, 0, 1000000> SPI;
#if (defined BOARD_ID_stm32_e407)
typedef USART_T<SYSCLK, USART_1> CON;

extern "C" {

void USART1_IRQHandler(void)
{
	if (CON::handle_irq()) exit_idle();
}

}

#endif

volatile bool nrf24_irq;

typedef NRF24_T<SPI, NRF24_CSN, NRF24_CE, NRF24_IRQ, SYSTICK, 0> NRF24;
typedef BOARD_T::BOARD<BOARD_T::BUTTON | BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::SPI | BOARD_T::CONSOLE> BOARD;

struct BTLE_ADV_PDU {
	uint8_t pdu_type;
	uint8_t pl_size;
	uint8_t mac[6];
	uint8_t payload[24];
};

struct BTLE_PDU_CHUNK {
	uint8_t size;
	uint8_t type;
	uint8_t data[];
};

static constexpr uint8_t month_lookup[24] = { 0,6,0,4,0,1,0,17,0,8,0,0,3,0,0,0,18,2,16,5,9,0,1,7 };
static constexpr uint8_t channel[3]   = {37,38,39};  // logical BTLE channel number (37-39)
static constexpr uint8_t frequency[3] = { 2,26,80};  // physical frequency (2400+x MHz)
static int current;
static constexpr uint8_t addr[5] = {0x71, 0x91, 0x7D, 0x6B, 0x00};
static BTLE_ADV_PDU buffer;

template<typename RADIO, const char NAME[], const size_t NAME_LEN>
struct BEACON_T {

#define month(m) month_lookup[ (( ((( (m[0] % 24) * 13) + m[1]) % 24) * 13) + m[2]) % 24 ]
#define chunk(x,y) ((BTLE_PDU_CHUNK*)(x.payload+y))

	static void init(void) {
		RADIO::init();
		RADIO::rw_reg(RF24_W_REGISTER + RF24_EN_AA, 0);
		RADIO::rw_reg(RF24_W_REGISTER + RF24_RF_SETUP, RF24_RF_0DBM);
		RADIO::rw_reg(RF24_W_REGISTER + RF24_CONFIG, 0);
		set_ble_channel(frequency[current]);
		RADIO::rw_reg(RF24_W_REGISTER + RF24_SETUP_RETR, 0);
		RADIO::rw_reg(RF24_W_REGISTER + RF24_SETUP_AW, 2);
		RADIO::write_reg(RF24_W_REGISTER + RF24_TX_ADDR, addr, 4);
		RADIO::write_reg(RF24_W_REGISTER + RF24_RX_ADDR_P0, addr, 4);
		RADIO::start_tx();
	}

	static void set_ble_channel(uint8_t num) {
		//current = MIN(2, MAX(0, num - 37));
		RADIO::set_channel(frequency[current]);
	}

	static void hop_channel(void) {
		current++;
		if (current >= sizeof(channel)) current = 0;
		set_ble_channel(frequency[current]);
	}


	static bool advertise(const void* buf, uint8_t buflen) {
		return advertise(0xFF, buf, buflen);
	}

	static bool advertise(uint8_t data_type, const void* buf, uint8_t buflen) {
		uint8_t pls = 0;

		buffer.mac[0] = ((__TIME__[6]-0x30) << 4) | (__TIME__[7]-0x30);
		buffer.mac[1] = ((__TIME__[3]-0x30) << 4) | (__TIME__[4]-0x30);
		buffer.mac[2] = ((__TIME__[0]-0x30) << 4) | (__TIME__[1]-0x30);
		buffer.mac[3] = ((__DATE__[4]-0x30) << 4) | (__DATE__[5]-0x30);
		buffer.mac[4] = month(__DATE__);
		buffer.mac[5] = ((__DATE__[9]-0x30) << 4) | (__DATE__[10]-0x30) | 0xC0;

		chunk(buffer,pls)->size = 0x02;
		chunk(buffer,pls)->type = 0x01;
		chunk(buffer,pls)->data[0]= 0x05;
		pls += 3;

		chunk(buffer,pls)->size = NAME_LEN+1;
		chunk(buffer,pls)->type = 0x09;
		for (uint8_t i = 0; i < NAME_LEN; i++)
			chunk(buffer,pls)->data[i] = NAME[i];
		pls += NAME_LEN+2;

		if (buflen > 0) {
			chunk(buffer,pls)->size = buflen+1;
			chunk(buffer,pls)->type = data_type;
			for (uint8_t i = 0; i < buflen; i++)
				chunk(buffer,pls)->data[i] = ((uint8_t*)buf)[i];
			pls += buflen+2;
		}

		if (pls > 21)
			return false;

		buffer.pdu_type = 0x42;
		buffer.pl_size = pls+6;

		uint8_t* outbuf = (uint8_t*)&buffer;
		crc(pls+8, outbuf+pls+8);

		whiten(pls+11);
		swapbuf(pls+11);

		RADIO::tx_buffer(outbuf, pls+11);

		return true;
	}

	static void whiten(uint8_t len) {
		uint8_t* buf = (uint8_t*)&buffer;
		uint8_t lfsr = channel[current] | 0x40;

		while (len--) {
			uint8_t res = 0;
			for (uint8_t i = 1; i; i <<= 1) {
				if (lfsr & 0x01) {
					lfsr ^= 0x88;
					res |= i;
				}
				lfsr >>= 1;
			}
			*(buf++) ^= res;
		}
	}

	static void crc(uint8_t len, uint8_t* dst) {
		uint8_t* buf = (uint8_t*)&buffer;

		dst[0] = 0xAA;
		dst[1] = 0xAA;
		dst[2] = 0xAA;
		while (len--) {
			uint8_t d = *(buf++);
			for (uint8_t i = 1; i; i <<= 1, d >>= 1) {
				uint8_t t = dst[0] & 0x01;         dst[0] >>= 1;
				if (dst[1] & 0x01) dst[0] |= 0x80; dst[1] >>= 1;
				if (dst[2] & 0x01) dst[1] |= 0x80; dst[2] >>= 1;
				if (t != (d & 1)) {
					dst[2] ^= 0xDA;
					dst[1] ^= 0x60;
				}
			}
		}
	}

	static void swapbuf(uint8_t len) {

		uint8_t* buf = (uint8_t*)&buffer;
		while (len--) {
			uint8_t a = *buf;
			uint8_t v = 0;
			if (a & 0x80) v |= 0x01;
			if (a & 0x40) v |= 0x02;
			if (a & 0x20) v |= 0x04;
			if (a & 0x10) v |= 0x08;
			if (a & 0x08) v |= 0x10;
			if (a & 0x04) v |= 0x20;
			if (a & 0x02) v |= 0x40;
			if (a & 0x01) v |= 0x80;
			*(buf++) = v;
		}
	}
};

static constexpr char const name[] = "MABUSE";
typedef BEACON_T<NRF24, name, 6> BEACON;

extern "C" {

void SPI1_IRQHandler(void)
{
	if (SPI::handle_irq()) exit_idle();
}

#if (defined STM32F1xx || defined STM32F4xx)
void EXTI0_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0>();
	exit_idle();
}

void EXTI1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<1>();
	exit_idle();
}

void EXTI2_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2>();
	exit_idle();
}

void EXTI3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<3>();
	exit_idle();
}

void EXTI4_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4>();
	exit_idle();
}

void EXTI9_5_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<5, 9>();
	exit_idle();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
	exit_idle();
}
#elif (defined STM32F0xx)
void EXTI0_1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0, 1>();
	exit_idle();
}

void EXTI2_3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2, 3>();
	exit_idle();
}

void EXTI4_15_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4, 15>();
	exit_idle();
}
#endif

void SysTick_Handler(void)
{
	if (TIMEOUT::count_down()) exit_idle();
}

}

void handle_radio_traffic(void)
{
        uint8_t pipe;
        int len;
        uint8_t buffer[33];

        if ((len = NRF24::template rx_buffer<TIMEOUT_IMMEDIATELY>(buffer, sizeof(buffer), &pipe)) > 0) {
                buffer[len] = '\0';
		CON::puts("(8080)");
                for (int i = 0; i < len; i++) {
                        printf<CON>("%02X", buffer[i]);
                }
		CON::putc('\n');
        }
}

uint8_t regs[64] __attribute__((aligned(4)));
uint8_t packet[64] __attribute__((aligned(4)));

int main(void)
{
	//memset(regs, 0, sizeof(regs));
	//memset(packet, 0, sizeof(packet));
	SYSCLK::init();
	SYSTICK::init();
	BOARD::init();
#if (defined BOARD_ID_stm32_e407)
	typedef GPIO_PORT_T<PE> PORT_E;
	typedef GPIO_PORT_T<PG> PORT_G;
	PORT_E::init();
	PORT_G::init();
	CON::init();
#endif
	LED1::set_high();
	SPI::init();
	NRF24_CSN::init();
	NRF24_CE::init();
	NRF24_IRQ::init();
	EXT_INTERRUPT::init();

	NRF24_CSN::set_high();
	NRF24_CE::set_low();
	BEACON::init();

	while (1) {
		LED1::toggle();
		BEACON::advertise("\x12\x34\x56\x78", 4);
		BEACON::hop_channel();
		TIMEOUT::set_and_wait(100);
	}
}
