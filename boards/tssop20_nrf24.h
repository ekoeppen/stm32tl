#pragma once

#include <uart.h>

typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_DOWN> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN> SWCLK;
typedef GPIO_PORT_T<PA, SWDIO, SWCLK> PORT_A;

typedef GPIO_PORT_T<PB> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
typedef GPIO_PORT_T<PD> PORT_D;
typedef GPIO_PORT_T<PF> PORT_F;

typedef GPIO_OUTPUT_T<PA, 1> LED1;
typedef PIN_UNUSED LED2;

typedef GPIO_OUTPUT_T<PA, 4> CSN;
typedef GPIO_T<PA, 2, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> TX;
typedef GPIO_T<PA, 5, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> SCK;
typedef GPIO_T<PA, 6, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> MISO;
typedef GPIO_T<PA, 7, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> MOSI;
typedef GPIO_T<PA, 0, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> BUTTON;
typedef GPIO_T<PA, 9, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SCL;
typedef GPIO_T<PA, 10, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SDA;

namespace BOARD_T {

enum {
	LED1 = 1,
	LED2 = 2,
	BUTTON = 4,
	CONSOLE = 8,
	I2C = 16,
	SPI = 32
};

template<const uint32_t FEATURES = LED1 | LED2 | BUTTON | CONSOLE | I2C | SPI>
struct BOARD {
	static void init(void) {
#ifdef STM32F0xx
		PORT_A::init();
#endif
		if (FEATURES & (LED1 | BUTTON | I2C | SPI | CONSOLE)) {
			PORT_A::init();
			if (FEATURES & LED1) LED1::init();
			if (FEATURES & BUTTON) BUTTON::init();
			if (FEATURES & I2C) {
				SCL::init();
				SDA::init();
			}
			if (FEATURES & CONSOLE) {
				TX::init();
			}
			if (FEATURES & SPI) {
				CSN::init();
				SCK::init();
				MOSI::init();
				MISO::init();
			}
		}
	}
};

};

