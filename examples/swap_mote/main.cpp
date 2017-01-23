#include <string.h>
#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <exti.h>
#include <io.h>
#include <spi.h>
#include <drivers/nrf24.h>
#include <drivers/swap_mote.h>

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
#ifdef STM32F0xx
typedef SPI_T<SYSCLK, true, 0, 1000000> SPI;
#else
typedef SPI_T<SYSCLK, SPI_1, true, 0, 1000000> SPI;
#endif
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

template<const uint32_t ID>
struct TICK_REGISTER_T {
	static uint16_t value;
	static constexpr uint8_t reg_id = ID;
	static bool write(SWAP_PACKET& packet) {
		packet.reg_id = ID;
		packet.reg_value[0] = value >> 8;
		packet.reg_value[1] = value & 0xff;
		packet.len = 7 + 2;
		return true;
	};
	static bool handle_command(SWAP_PACKET& packet) { return false; }
	static bool handle_query(SWAP_PACKET& packet) { return false; }
	static void update(void) { value++; };
};

template<const uint32_t ID>
uint16_t TICK_REGISTER_T<ID>::value = 0;

typedef TICK_REGISTER_T<11> TICKER;
typedef NRF24_T<SPI, NRF24_CSN, NRF24_CE, NRF24_IRQ, TIMEOUT> NRF24;
typedef BOARD_T::BOARD<BOARD_T::BUTTON | BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::SPI | BOARD_T::CONSOLE> BOARD;
typedef SWAP_MOTE_T<1, 1, 1, 1, NRF24, 70, CONFIG_STORAGE_UNUSED, TICKER> SWAP_MOTE;

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
		LED1::toggle();
                buffer[len] = '\0';
		CON::puts("(8080)");
                for (int i = 0; i < len; i++) {
                        printf<CON>("%02X", buffer[i]);
                }
		CON::putc('\n');
        }
}

bool handle_status_packets(void)
{
	uint8_t *buffer = reinterpret_cast<uint8_t *>(&SWAP_MOTE::rx_packet);
	LED1::toggle();
	for (int i = 0; i < SWAP_MOTE::rx_packet.len; i++) {
		printf<CON>("%02X", buffer[i]);
	}
	CON::putc('\n');

	return false;
}

int main(void)
{
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
	NRF24::init();

	SWAP_MOTE::init();
	NRF24::set_rx_addr(rx_addr);
	NRF24::set_channel(70);
	NRF24::start_rx();
#if 1
	while (1) {
		/*
		SWAP_MOTE::update_registers();
		SWAP_MOTE::transmit_data();
		*/

		CON::puts("Start\n");
		SWAP_MOTE::start_tx();
		NRF24::tx_buffer((const uint8_t *) "\x01\xff\x00\x00\x01\x01\x81", 7);
		SWAP_MOTE::start_rx();
		TIMEOUT::set(3000);
		while (!TIMEOUT::triggered()) {
			SWAP_MOTE::handle_radio<TIMEOUT, handle_status_packets>();
		}
		TIMEOUT::disable();
		NRF24::power_down();
		CON::puts("Stop\n");

		/*
		TIMEOUT::set(3000);
		BUTTON::wait_for_irq<TIMEOUT>();
		if (BUTTON::irq_raised()) {
			BUTTON::clear_irq();
		}
		TIMEOUT::disable();
		*/
		TIMEOUT::set_and_wait(3000);
	}
#else
	while (1) {
		SWAP_MOTE::start_rx();
		TIMEOUT::set(3000);
		while (!TIMEOUT::triggered()) {
			SWAP_MOTE::handle_radio<TIMEOUT, handle_status_packets>();
		}
		TIMEOUT::disable();
		NRF24::power_down();
	}
#endif
}
