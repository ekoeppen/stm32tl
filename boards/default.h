#pragma once

#include <clocks.h>
#include <tasks.h>
#include <drivers/ringbuffer.h>

typedef PIN_UNUSED LED1;
typedef PIN_UNUSED LED2;
typedef PIN_UNUSED BUTTON;

struct CON {
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
	}
};

};
