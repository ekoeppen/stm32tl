#pragma once

#include <stm32f1xx.h>

enum {
	DS_REGULATOR_NORMAL,
	DS_REGULATOR_LOW_POWER
};

enum {
	DS_STOP,
	DS_STANDBY
};

enum {
	DS_DISABLED,
	DS_ENABLED
};

template<const int REGULATOR = DS_REGULATOR_NORMAL,
	const int STOP_MODE = DS_STOP>
struct PWR_T {
	static void init(void) {
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |=
			(REGULATOR == DS_REGULATOR_LOW_POWER ? PWR_CR_LPDS : 0) |
			(STOP_MODE == DS_STANDBY ? PWR_CR_PDDS : 0);
	}

	static void sleep(void) {
		SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
		__WFI();
	}

	static void stop(void) {
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
		__WFI();
	}
};
