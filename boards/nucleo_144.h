#pragma once

#include <uart.h>

typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_DOWN> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN> SWCLK;

typedef GPIO_PORT_T<PA, SWDIO, SWCLK> PORT_A;
typedef GPIO_PORT_T<PB> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
typedef GPIO_PORT_T<PD> PORT_D;
typedef GPIO_PORT_T<PF> PORT_F;
typedef GPIO_PORT_T<PG> PORT_G;
typedef GPIO_PORT_T<PH> PORT_H;
typedef GPIO_PORT_T<PI> PORT_I;

typedef GPIO_T<PA, 0, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> BUTTON;
typedef PIN_UNUSED LED2;
struct LED1: GPIO_OUTPUT_T<PC, 13> {
	static void set_high(void) {
		port->BSRRH = bit_value;
	}

	static void set_low(void) {
		port->BSRRL = bit_value;
	}

	static void set_value(const int value) {
		if (value) set_high(); else set_low();
	}

	static void toggle(void) {
		if (port->ODR & bit_value) set_low(); else set_high();
	}

};
typedef GPIO_T<PB, 8, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF4> SCL;
typedef GPIO_T<PB, 9, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF4> SDA;
typedef GPIO_T<PA, 5, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF5> SCK;
typedef GPIO_T<PA, 6, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF5> MISO;
typedef GPIO_T<PB, 5, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF5> MOSI;
typedef GPIO_T<PB, 6, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF7> TX;
typedef GPIO_T<PB, 7, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF7> RX;

struct CON_DISABLED {
	static void init(void) { }
	static bool enabled(void) { return false; }
	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void write(uint8_t *, uint32_t) { }
	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint32_t read(uint8_t *, uint32_t) { return 0; }
	template<typename TIMEOUT = TIMEOUT_NEVER>
	static char getc(void) { return 0; }
	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void putc(char) { }
	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint32_t gets(char *, uint32_t) { return 0; }
	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void puts(const char *) { }
	static bool handle_irq(void) { return true; }
};

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
		PORT_A::init();
		if (FEATURES & BUTTON) {
			BUTTON::init();
		}
		if (FEATURES & (SPI | CONSOLE)) {
			PORT_B::init();
			if (FEATURES & SPI) {
				SCK::init();
				MOSI::init();
				MISO::init();
			}
			if (FEATURES & CONSOLE) {
				TX::init();
				RX::init();
			}
		}
		if (FEATURES & LED1) {
			LED1::init();
		}
		if (FEATURES & I2C) {
			PORT_B::init();
			SCL::init();
			SDA::init();
		}
	}
};

}

