#include "common.h"

int main(void)
{
	BOARD::init();
	SYSCLK::init();
	SYSTICK::init();
	CC1101_IRQ::init();
	MISO_IN::init();
	LED1::set_high();
	EXT_INTERRUPT::init();
	CC1101::reset();
	CC1101::init(cc110x_default_init_values, ARRAY_COUNT(cc110x_default_init_values));
	LED2::set_high();
	while (1) {
		LED2::toggle();
		CC1101::tx_buffer((uint8_t *) "\x55\xaa\x55\xaa", 4);
		SYSTICK::set_and_wait(1000);
	}
}
