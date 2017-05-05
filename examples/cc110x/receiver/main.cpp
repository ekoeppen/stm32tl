// #define OLIMEXINO

#include <common.h>

uint8_t packet[64];

int main(void)
{
	BOARD::init();
	SYSCLK::init();
	SYSTICK::init();
	SPI::init();
	CC1101_IRQ::init();
	CC1101_CSN::init();
	CC1101_CSN::set_high();
	MISO_IN::init();
	LED2::set_high();
	EXT_INTERRUPT::init();
	CC1101::reset();
	MISO::init();
	CC1101::init(cc110x_default_init_values, ARRAY_COUNT(cc110x_default_init_values));
	CC1101::start_rx();
	LED2::set_low();
	while (1) {
		CC1101::template rx_buffer<TIMEOUT_NEVER>(packet, sizeof(packet));
		LED1::set_high();
		SYSTICK::set_and_wait(50);
		LED1::set_low();
	}
}
