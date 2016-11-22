// #define OLIMEXINO

#include <common.h>

uint8_t packet[64];

int main(void)
{
	HSE::init();
	PLL::init();
	SYSTICK::init();
	SYSCLK::init();
	POWER::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	LED_GREEN::set_high();
	LED_RED::set_low();
	EXT_INTERRUPT::init();
	SPI::init();
	CC1101::reset();
	CC1101::init(cc110x_default_init_values, ARRAY_COUNT(cc110x_default_init_values));
	CC1101::start_rx();
	LED_GREEN::set_low();
	while (1) {
		CC1101::template rx_buffer<TIMEOUT_NEVER>(packet, sizeof(packet));
		LED_RED::set_high();
		SYSTICK::set_and_wait(50);
		LED_RED::set_low();
	}
}
