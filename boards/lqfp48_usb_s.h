#pragma once

#include <usb/usb.h>
#include <drivers/usb_cdc.h>
#include <drivers/ringbuffer.h>

#ifdef STM32F0xx
typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_DOWN> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN> SWCLK;
typedef GPIO_PORT_T<PA, SWDIO, SWCLK> PORT_A;
#else
typedef GPIO_PORT_T<PA> PORT_A;
#endif

typedef GPIO_PORT_T<PB> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
typedef GPIO_PORT_T<PD> PORT_D;
typedef GPIO_PORT_T<PF> PORT_F;

typedef GPIO_OUTPUT_T<PB, 8> LED1;
typedef GPIO_OUTPUT_T<PB, 9> LED2;

#ifdef STM32F0xx
typedef GPIO_T<PA, 5, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> SCK;
typedef GPIO_T<PA, 6, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> MISO;
typedef GPIO_T<PA, 7, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> MOSI;
typedef GPIO_T<PB, 7, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> BUTTON;
typedef GPIO_T<PB, 10, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SCL;
typedef GPIO_T<PB, 11, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SDA;
#else
typedef GPIO_T<PA, 5, OUTPUT_10MHZ, ALT_PUSH_PULL> SCK;
typedef GPIO_T<PA, 6, OUTPUT_10MHZ, ALT_PUSH_PULL> MISO;
typedef GPIO_T<PA, 7, OUTPUT_10MHZ, ALT_PUSH_PULL> MOSI;
typedef GPIO_T<PB, 7, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PB, 10, OUTPUT_10MHZ, ALT_OPENDRAIN> SCL;
typedef GPIO_T<PB, 11, OUTPUT_10MHZ, ALT_OPENDRAIN> SDA;
#endif

typedef USB_T::CDC CON;

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
		if (FEATURES & (LED1 | LED2 | BUTTON | I2C)) {
			PORT_B::init();
			if (FEATURES & LED1) LED1::init();
			if (FEATURES & LED2) LED2::init();
			if (FEATURES & BUTTON) BUTTON::init();
			if (FEATURES & I2C) {
				SCL::init();
				SDA::init();
			}
		}
		if (FEATURES & CONSOLE) {
			CON::init();
		}
		if (FEATURES & SPI) {
			PORT_A::init();
			SCK::init();
			MOSI::init();
			MISO::init();
		}
	}
};

};

extern "C" {

#ifdef STM32F0xx
void USB_IRQHandler(void) {
#else
void USB_LP_CAN1_RX0_IRQHandler(void) {
#endif
	if (CON::handle_irq()) exit_idle();
}

}
