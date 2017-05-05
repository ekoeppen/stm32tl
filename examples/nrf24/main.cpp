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
#elif (defined BOARD_ID_tssop20_nrf24)
typedef GPIO_T<PA, 3, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> NRF24_IRQ;
typedef GPIO_OUTPUT_T<PB, 1> NRF24_CE;
typedef CSN NRF24_CSN;
#endif

typedef EXTI_T<NRF24_IRQ, BUTTON> EXT_INTERRUPT;
#ifdef STM32F0xx
typedef SPI_T<SYSCLK, true, 0, 1000000> SPI;
#else
typedef SPI_T<SYSCLK, SPI_1, true, 0, 1000000> SPI;
#endif
#if (defined BOARD_ID_stm32_e407 || defined BOARD_ID_tssop20_nrf24)
typedef USART_T<SYSCLK, USART_1, 115200> CON;

extern "C" {

void USART1_IRQHandler(void)
{
	if (CON::handle_irq()) exit_idle();
}

}

#endif

volatile bool nrf24_irq;

//typedef NRF24_T<SPI, NRF24_CSN, NRF24_CE, NRF24_IRQ, TIMEOUT> NRF24;
typedef NRF24_T<SPI, NRF24_CSN, NRF24_CE, PIN_UNUSED, TIMEOUT> NRF24;
typedef BOARD_T::BOARD<BOARD_T::BUTTON | BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::SPI | BOARD_T::CONSOLE> BOARD;

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
static const uint8_t test_data[9] = {
	0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x55
};

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
#elif (defined BOARD_ID_tssop20_nrf24)
	PORT_B::init();
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
	NRF24::set_rx_addr(rx_addr);
	NRF24::set_channel(70);
	NRF24::start_rx();

	NRF24::read_regs(regs); hex_dump<CON>(regs, 41);
	while (1) {
		handle_radio_traffic();
		if (BUTTON::irq_raised()) {
			LED1::toggle();
			NRF24::start_tx();
			NRF24::tx_buffer(test_data, sizeof(test_data));
			NRF24::start_rx();
			BUTTON::clear_irq();
		}
	}
}
