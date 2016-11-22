#pragma once

#include <stm32f1xx.h>

enum CLOCK_SOURCE_TYPE {
	HSI_CLOCK_SOURCE,
	HSE_CLOCK_SOURCE,
	LSI_CLOCK_SOURCE,
	LSE_CLOCK_SOURCE,
	PLL_CLOCK_SOURCE
};

struct HSI_RC {
	static constexpr CLOCK_SOURCE_TYPE clock_source = HSI_CLOCK_SOURCE;
	static constexpr uint32_t frequency = 8000000;
	static void init(void) {
		RCC->CR |= RCC_CR_HSION;
		while (!(RCC->CR & RCC_CR_HSIRDY));
	}

	static void enable(void) {
		init();
	}

	static void disable(void) {
		RCC->CR &= ~RCC_CR_HSION;
	}

	static void claim(void) { }
	static void release(void) { }
};

template<const uint32_t FREQUENCY = 8000000>
struct HSE_OSC_T {
	static constexpr CLOCK_SOURCE_TYPE clock_source = HSE_CLOCK_SOURCE;
	static constexpr uint32_t frequency = FREQUENCY;

	static void init(void) {
		RCC->CR |= RCC_CR_HSEON;
		while (!(RCC->CR & RCC_CR_HSERDY));
	}

	static void enable(void) {
		init();
	}

	static void disable(void) {
		RCC->CR &= ~RCC_CR_HSEON;
	}

	static void claim(void) { }
	static void release(void) { }
};

struct LSI_RC_T {
	static constexpr CLOCK_SOURCE_TYPE clock_source = LSI_CLOCK_SOURCE;
	static constexpr uint32_t frequency = 40000;
	static void init(void) {
		RCC->CSR |= RCC_CSR_LSION;
		while (!(RCC->CSR & RCC_CSR_LSIRDY));
	}


	static void claim(void) { }
	static void release(void) { }
};

template<const uint32_t FREQUENCY>
struct LSE_OSC_T {
	static constexpr CLOCK_SOURCE_TYPE clock_source = LSE_CLOCK_SOURCE;
	static constexpr uint32_t frequency = FREQUENCY;
	static void init(void) {
		PWR->CR |= PWR_CR_DBP;
		RCC->BDCR |= RCC_BDCR_LSEON;
		while (!(RCC->BDCR & RCC_BDCR_LSERDY));
	}

	static void claim(void) { }
	static void release(void) { }
};

template<typename CLOCK_SOURCE,
	const uint32_t FREQUENCY>
struct PLL_T {
	static constexpr CLOCK_SOURCE_TYPE clock_source = PLL_CLOCK_SOURCE;
	static constexpr uint32_t frequency = FREQUENCY;
	static constexpr uint32_t multiplier(const uint32_t source_frequency, const uint32_t frequency) {
		return (frequency / source_frequency - 2) << 18;
	}

	static void init(void) {
		RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | multiplier(CLOCK_SOURCE::frequency, frequency));
		RCC->CR |= RCC_CR_PLLON;
		while(!(RCC->CR & RCC_CR_PLLRDY));
	}

	static void claim(void) { }
	static void release(void) { }
};

template<typename CLOCK_SOURCE = HSI_RC,
	const uint8_t AHB_PRESCALER = 1,
	const uint8_t APB1_PRESCALER = 2,
	const uint8_t APB2_PRESCALER = 1,
	const uint8_t ADC_PRESCALER = 2,
	const bool USB_PRESCALER = true,
	const bool CSS_ENABLED = false>
struct SYSCLK_T {
	static constexpr uint32_t frequency = CLOCK_SOURCE::frequency;
	static constexpr uint32_t hclk = frequency / AHB_PRESCALER;
	static constexpr uint32_t pclk1 = frequency / APB1_PRESCALER;
	static constexpr uint32_t pclk2 = frequency / APB2_PRESCALER;
	static void init(void) {
		FLASH->ACR |= FLASH_ACR_PRFTBE;
		if (frequency > 48000000) {
			FLASH->ACR &= ~FLASH_ACR_LATENCY;
			FLASH->ACR |= FLASH_ACR_LATENCY_2;
		} else if (frequency > 24000000) {
			FLASH->ACR &= ~FLASH_ACR_LATENCY;
			FLASH->ACR |= FLASH_ACR_LATENCY_1;
		}
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2;
		static_assert(AHB_PRESCALER == 1, "AHB prescaler setting not implemented");
		static_assert(APB1_PRESCALER == 2, "APB1 prescaler setting not implemented");
		static_assert(APB2_PRESCALER == 1, "APB2 prescaler setting not implemented");
		switch (CLOCK_SOURCE::clock_source) {
		case HSI_CLOCK_SOURCE:
			break;
		case HSE_CLOCK_SOURCE:
			RCC->CFGR &= ~RCC_CFGR_SW;
			RCC->CFGR |= RCC_CFGR_SW_HSE;
			while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);
		case LSI_CLOCK_SOURCE:
		case LSE_CLOCK_SOURCE:
			break;
		case PLL_CLOCK_SOURCE:
			RCC->CFGR &= ~RCC_CFGR_SW;
			RCC->CFGR |= RCC_CFGR_SW_PLL;
			while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
			break;
		}

	}

	static void claim(void) { CLOCK_SOURCE::claim(); }
	static void release(void) { CLOCK_SOURCE::release(); }
};

void enter_idle(void)
{
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
	__WFE();
}

void exit_idle(void)
{
	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
	__SEV();
}

template<typename SYSCLK, const uint32_t FREQUENCY = 1000>
struct SYSTICK_T {
	static constexpr uint32_t frequency = FREQUENCY;
	static constexpr uint32_t reload = SYSCLK::frequency / FREQUENCY;
	static int8_t users;

	static void claim(void) {
		enable_irq();
		users++;
	}

	static void release(void) {
		users--;
		if (users == 0) disable_irq();
	}

	static void init(void) {
		SysTick->LOAD = reload;
		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
	}

	static void enable_irq(void) {
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	}

	static void disable_irq(void) {
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	}

	static void set_and_wait_us(uint32_t microseconds) {
		uint32_t delta = microseconds * reload / FREQUENCY;
		uint32_t then;
		if (delta + 100 < SysTick->VAL) {
			then = SysTick->VAL - delta;
		} else {
			then = reload + SysTick->VAL - delta;
			while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
		}
		while (SysTick->VAL > then);
	}

	static void set_and_wait(uint32_t milliseconds) {
		milliseconds = milliseconds * FREQUENCY / 1000;
		while (milliseconds--) {
			while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
		}
	}
};

template<typename SYSCLK, const uint32_t FREQUENCY>
int8_t SYSTICK_T<SYSCLK, FREQUENCY>::users = 0;
