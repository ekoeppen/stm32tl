#include "common.h"

static const uint8_t data[7] = {0, 0, 0, 0, 0, 0, 0};

static uint8_t buffer[64];

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
	LED1::set_high();
	EXT_INTERRUPT::init();
	CC1101::reset();
	MISO::init();
	CC1101::init(cc110x_default_init_values, ARRAY_COUNT(cc110x_default_init_values));
	/*
	CC1101_CSN::set_low();
	SPI::transfer(CC1101_PATABLE);
	SPI::transfer(0x03);
	SPI::transfer(0xc0);
	CC1101_CSN::set_high();
	*/
	CC1101::write_burst_reg(CC1101_PATABLE, (uint8_t *) "\x00\xc0\x00\x00\x00\x00\x00\x00", 8);
	CC1101::write_reg(CC1101_FREND0, 0x11);
	CC1101::read_regs(buffer);
	LED1::set_low();
	while (1) {
		LED2::toggle();
		CC1101::tx_buffer(data, sizeof(data));
		SYSTICK::set_and_wait(1000);
	}
}
