#include <string.h>
#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <spi.h>
#include <stddef.h>
#include <io.h>
#include <exti.h>

#include <drivers/ringbuffer.h>
#include <usb/usb.h>
#include <drivers/usb_cdc.h>

#include <board.h>

// #include "radio_config_Si4455.h"

#if (MODE == TX)
#include "radio_config_si4455_std_pkt_tx.h"
#else
#include "radio_config_si4355_std_pkt_rx.h"
#endif

#ifdef STM32F0xx
typedef SYSCLK_T<HSI48_RC> SYSCLK;
#else
typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
#endif
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;


typedef GPIO_OUTPUT_T<PA, 4> CSN;
#ifdef STM32F0xx
typedef GPIO_T<PB, 1, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> IRQ;
#else
#ifdef BOARD_ID_olimexino_stm32
typedef GPIO_T<PC, 1, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> IRQ;
#else
typedef GPIO_T<PB, 1, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> IRQ;
#endif
#endif
#ifdef BOARD_ID_olimexino_stm32
typedef GPIO_INPUT_T<PC, 0> CTS;
#else
typedef GPIO_INPUT_T<PB, 0> CTS;
#endif
typedef GPIO_OUTPUT_T<PA, 8> SDN;

#ifdef STM32F0xx
typedef SPI_T<SYSCLK, true, 0> SPI;
#else
typedef SPI_T<SYSCLK, SPI_1, true, 0, 4000000> SPI;
#endif

typedef EXTI_T<IRQ> EXT_INTERRUPT;

typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON /*| BOARD_T::CONSOLE */ | BOARD_T::SPI> BOARD;

static uint8_t const radio_config[] = RADIO_CONFIGURATION_DATA_ARRAY;
static uint8_t response_buffer[64];

void poll_cts(void) {
	uint8_t response;
	do {
		SPI::transfer(0x44);
		response = SPI::transfer(0xff);
	} while (response == 0x00);
	response = SPI::transfer(0xff);
}

void radio_init(void)
{
	const uint8_t *data = radio_config;
	uint8_t len, response;
	while ((len = *data++) > 0) {
		while (CTS::is_low());
		CSN::set_low();
		SPI::transfer((uint8_t *) data, len);
		CSN::set_high();
		if (*data != 0x66) {
			while (CTS::is_low());
			do {
				CSN::set_low();
				SPI::transfer(0x44);
				response = SPI::transfer(0xff);
				CSN::set_high();
			} while (response != 0xff);
		}
		data += len;
	}
}

void radio_command(uint8_t *command, uint8_t length, uint8_t *response, uint8_t response_length)
{
	uint8_t r, done;

	while (CTS::is_low());
	CSN::set_low();
	while (length--) {
		SPI::transfer(*command++);
	}
	CSN::set_high();
	done = false;
	while (!done) {
		while (CTS::is_low());
		CSN::set_low();
		SPI::transfer(0x44);
		r = SPI::transfer(0xff);
		if (r == 0xff) {
			while (response_length--) {
				*response++ = SPI::transfer(0xff);
			}
			done = true;
		}
		CSN::set_high();
	}
}

void radio_command(uint8_t command, uint8_t *response, uint8_t response_length)
{
	radio_command(&command, 1, response, response_length);
}

void read_rx_fifo(uint8_t *response, uint8_t response_length)
{
	CSN::set_low();
	SPI::transfer(0x77);
	while (response_length--) {
		*response++ = SPI::transfer(0xff);
	}
	CSN::set_high();
}

void write_tx_fifo(uint8_t *packet, uint8_t packet_length)
{
	CSN::set_low();
	SPI::transfer(0x66);
	while (packet_length--) {
		SPI::transfer(*packet++);
	}
	CSN::set_high();
}

void read_rx_fifo(uint8_t length)
{
	read_rx_fifo(response_buffer, length); // hex_dump<CON>(response_buffer, length, "RX FIFO");
}

void start_rx(void)
{
	radio_command((uint8_t *) "\x32\x00\x00\x00\x07\x00\x01\x00", 8, 0, 0);
}

void start_tx(uint8_t len)
{
	uint8_t buffer[5];

	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0x31;
	buffer[2] = 0x30;
	buffer[4] = len;
	radio_command(buffer, sizeof(buffer), 0, 0);
}

void rx_fifo_info(void)
{
	radio_command((uint8_t *) "\x15\x00", 2, response_buffer, 1); // hex_dump<CON>(response_buffer, 16, "FIFO Status");
}

void reset_rx_fifo(void)
{
	radio_command((uint8_t *) "\x15\x02", 2, response_buffer, 1); // hex_dump<CON>(response_buffer, 16, "FIFO Status (reset)");
}

void part_info(void)
{
	radio_command(0x01, response_buffer, 8); // hex_dump<CON>(response_buffer, 16, "Device status");
}

void device_status(void)
{
	radio_command(0x33, response_buffer, 2); // hex_dump<CON>(response_buffer, 16, "Device status");
}

void clear_irq(void)
{
	radio_command((uint8_t *) "\x20\x00\x00\x00", 4, response_buffer, 8); // hex_dump<CON>(response_buffer, 8, "IRQ Status");
}

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
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
}

int main(void)
{
#ifdef STM32F0xx
	HSI48_RC::init();
#else
	HSE::init();
	PLL::init();
#endif
	SYSCLK::init();
	SYSTICK::init();
	BOARD::init();

	CSN::init(); IRQ::init(); CTS::init(); SDN::init();
	SPI::init();

	CSN::set_high();
	SDN::set_high();
	TIMEOUT::set_and_wait(1);
	SDN::set_low();
	TIMEOUT::set_and_wait(5);
	EXT_INTERRUPT::init();

	// CON::getc();
	LED1::set_high();
	// CON::puts("Si4455 example start.\n");
	radio_init();
	part_info();
	while (1) {
#if (MODE == RX)
		clear_irq();
		start_rx();
		device_status();
		// CON::puts("-- Wait for IRQ ------------------------------------------\n");
		IRQ::clear_irq();
		IRQ::wait_for_irq();
		clear_irq();
		LED2::set_high();
		rx_fifo_info();
		read_rx_fifo(response_buffer[0]);
		reset_rx_fifo();
		// CON::puts("----------------------------------------------------------\n");
		LED2::set_low();
#else
		LED2::set_high();
		write_tx_fifo((uint8_t *) "\x03\x00\x11\x22\x5a\xe8", 6);
		start_tx(6);
		IRQ::clear_irq();
		IRQ::wait_for_irq();
		clear_irq();
		LED2::set_low();
		TIMEOUT::set_and_wait(200);
#endif
	}
}
