#pragma once

#include <stm32f1xx.h>
#include <clocks.h>

template<typename CLOCK, const uint32_t FREQUENCY = 1>
struct RTC_T {
	static constexpr uint32_t frequency = FREQUENCY;

	static constexpr uint8_t clock_source(void) {
		return (CLOCK::clock_source == LSE_CLOCK_SOURCE ? 0b01 :
			(CLOCK::clock_source == LSI_CLOCK_SOURCE ? 0b10 :
			(CLOCK::clock_source == HSE_CLOCK_SOURCE ? 0b11 : 0b00)));
	}

	static constexpr uint32_t prescaler(void) {
		return CLOCK::frequency / (CLOCK::clock_source == HSE_CLOCK_SOURCE ? 128 : 1) / FREQUENCY - 1;
	}

	static void init() {
		RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
		PWR->CR |= PWR_CR_DBP;
		RCC->BDCR |= RCC_BDCR_RTCEN | (clock_source() << 8);
		RTC->CRL |= RTC_CRL_CNF;
		RTC->PRLH = prescaler() >> 16;
		RTC->PRLL = prescaler() & 0xffff;
		RTC->CRH |= RTC_CRH_SECIE;
		RTC->CRL &= ~RTC_CRL_CNF;
		NVIC_ClearPendingIRQ(RTC_IRQn);
		NVIC_EnableIRQ(RTC_IRQn);
	}

	static void claim(void) { }
	static void release(void) { }

	static void wait_for_sync(void) {
		RTC->CRL &= ~RTC_CRL_RSF;
		while (!(RTC->CRL & RTC_CRL_RSF));
	}

	static void handle_irq(void) {
		NVIC_ClearPendingIRQ(RTC_IRQn);
		RTC->CRL &= ~RTC_CRL_SECF;
	}

	static constexpr int exti_source(void) {
		return 1 << 17;
	}

	static constexpr int exti_trigger_falling(void) {
		return 0;
	}

	static constexpr int exti_trigger_rising(void) {
		return 1 << 17;
	}

	static constexpr int exti_syscfg(const int i) {
		return 0;
	}

	static constexpr int exti_cr(const int i) {
		return 0;
	}
};
