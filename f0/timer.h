#ifndef __TIMER_H
#define __TIMER_H

static constexpr TIM_TypeDef *timer_addr[18] = {
	0, TIM1, 0, TIM3, 0, 0, TIM6, 0, 0, 0, 0, 0, 0, 0, TIM14, TIM15, TIM16, TIM17
};

template <const int TIMER_NUMBER,
	 const int CHANNEL_NUMBER,
	 const int FILTER = 0,
	 const int PRESCALER = 0,
	 const int INPUT_SOURCE = 1,
	 const int EDGE = EDGE_RISING>
struct INPUT_TIMER_CHANNEL_T {
	static constexpr TIM_TypeDef *timer = timer_addr[TIMER_NUMBER];
	static constexpr int number = CHANNEL_NUMBER - 1;
	static constexpr bool input = true;
	static constexpr bool output = false;
	static constexpr int filter = FILTER;
	static constexpr int prescaler = PRESCALER;
	static constexpr int edge = EDGE;

	static void enable(void) {
		timer->CCR1 = 0;
		timer->CCER &= ~(0xf << number * 4);
		timer->CCER |= (1 << number * 4) |
			(edge == EDGE_FALLING ? 0x2 << number : 0) |
			(edge == EDGE_BOTH ? 0xa << number : 0);
	}

	static void disable(void) {
		timer->CCER &= ~(1 << number * 4);
	}

	static void set_mode(void) {
		uint32_t mode = 0;

		mode = INPUT_SOURCE | prescaler << 2 | filter << 4;
		switch (number) {
			case 0: timer->CCMR1 = mode; break;
			case 1: timer->CCMR1 = mode << 8; break;
			case 2: timer->CCMR2 = mode; break;
			case 3: timer->CCMR2 = mode << 8; break;
		}
	}

	static void enable_irq(void) {
		timer->DIER |= (2 << number);
		switch (TIMER_NUMBER) {
			case 1: NVIC_ClearPendingIRQ(TIM1_BRK_UP_TRG_COM_IRQn); NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
				NVIC_ClearPendingIRQ(TIM1_CC_IRQn); NVIC_EnableIRQ(TIM1_CC_IRQn); break;
			case 3: NVIC_ClearPendingIRQ(TIM3_IRQn); NVIC_EnableIRQ(TIM3_IRQn); break;
			case 6: NVIC_ClearPendingIRQ(TIM6_DAC_IRQn); NVIC_EnableIRQ(TIM6_DAC_IRQn); break;
			case 14: NVIC_ClearPendingIRQ(TIM14_IRQn); NVIC_EnableIRQ(TIM14_IRQn); break;
			case 15: NVIC_ClearPendingIRQ(TIM15_IRQn); NVIC_EnableIRQ(TIM15_IRQn); break;
			case 16: NVIC_ClearPendingIRQ(TIM16_IRQn); NVIC_EnableIRQ(TIM16_IRQn); break;
			case 17: NVIC_ClearPendingIRQ(TIM17_IRQn); NVIC_EnableIRQ(TIM17_IRQn); break;
		}
	}

	static void disable_irq(void) {
	}

};

struct UNUSED_CHANNEL {
	static constexpr int number = -1;
};

template <const int TIMER_NUMBER,
	 typename CH1 = UNUSED_CHANNEL,
	 typename CH2 = UNUSED_CHANNEL,
	 typename CH3 = UNUSED_CHANNEL,
	 typename CH4 = UNUSED_CHANNEL>
struct TIMER_T {
	static constexpr TIM_TypeDef *timer = timer_addr[TIMER_NUMBER];

	static void init(void) {
		switch (TIMER_NUMBER) {
			case 1: RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; break;
			case 3: RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; break;
			case 6: RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; break;
			case 14: RCC->APB1ENR |= RCC_APB1ENR_TIM14EN; break;
			case 15: RCC->APB2ENR |= RCC_APB2ENR_TIM15EN; break;
			case 16: RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; break;
			case 17: RCC->APB2ENR |= RCC_APB2ENR_TIM17EN; break;
		}
	}

	static void disable(void) {
		switch (TIMER_NUMBER) {
			case 1: RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN; break;
			case 3: RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN; break;
			case 6: RCC->APB1ENR &= ~RCC_APB1ENR_TIM6EN; break;
			case 14: RCC->APB1ENR &= ~RCC_APB1ENR_TIM14EN; break;
			case 15: RCC->APB2ENR &= ~RCC_APB2ENR_TIM15EN; break;
			case 16: RCC->APB2ENR &= ~RCC_APB2ENR_TIM16EN; break;
			case 17: RCC->APB2ENR &= ~RCC_APB2ENR_TIM17EN; break;
		}
	}

	static void counter_start(void) {
		timer->CNT = 0;
		timer->CR1 |= TIM_CR1_CEN;
	}

	static void counter_stop(void) {
		timer->CR1 &= ~TIM_CR1_CEN;
	}

	template <typename CHANNEL>
	static void enable_channel(void) {
		timer->CCER &= ~(0xf << CHANNEL::number * 4);
		timer->CCER |= (1 << CHANNEL::number * 4) |
			(CHANNEL::edge == EDGE_FALLING ? 0x2 << CHANNEL::number : 0) |
			(CHANNEL::edge == EDGE_BOTH ? 0xa << CHANNEL::number : 0);
	}
};

#endif
