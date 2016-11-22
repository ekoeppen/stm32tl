#pragma once

volatile uint32_t EXTI_IRQ_STATE;

struct EXTI_SOURCE_UNUSED {
	static constexpr int exti_source(void) {
		return 0;
	}

	static constexpr int exti_trigger_rising(void) {
		return 0;
	}

	static constexpr int exti_trigger_falling(void) {
		return 0;
	}

	static constexpr int exti_cr(const int i) {
		return 0;
	}
};

template<
typename SOURCE0,
	 typename SOURCE1 = EXTI_SOURCE_UNUSED,
	 typename SOURCE2 = EXTI_SOURCE_UNUSED,
	 typename SOURCE3 = EXTI_SOURCE_UNUSED,
	 typename SOURCE4 = EXTI_SOURCE_UNUSED,
	 typename SOURCE5 = EXTI_SOURCE_UNUSED,
	 typename SOURCE6 = EXTI_SOURCE_UNUSED,
	 typename SOURCE7 = EXTI_SOURCE_UNUSED,
	 typename SOURCE8 = EXTI_SOURCE_UNUSED,
	 typename SOURCE9 = EXTI_SOURCE_UNUSED,
	 typename SOURCE10 = EXTI_SOURCE_UNUSED,
	 typename SOURCE11 = EXTI_SOURCE_UNUSED,
	 typename SOURCE12 = EXTI_SOURCE_UNUSED,
	 typename SOURCE13 = EXTI_SOURCE_UNUSED,
	 typename SOURCE14 = EXTI_SOURCE_UNUSED,
	 typename SOURCE15 = EXTI_SOURCE_UNUSED,
	 typename SOURCE16 = EXTI_SOURCE_UNUSED,
	 typename SOURCE17 = EXTI_SOURCE_UNUSED>
struct EXTI_T {
	static constexpr uint32_t interrupt_mask =
		SOURCE0::exti_source() | SOURCE1::exti_source() | SOURCE2::exti_source() | SOURCE3::exti_source() |
		SOURCE4::exti_source() | SOURCE5::exti_source() | SOURCE6::exti_source() | SOURCE7::exti_source() |
		SOURCE8::exti_source() | SOURCE9::exti_source() | SOURCE10::exti_source() | SOURCE11::exti_source() |
		SOURCE12::exti_source() | SOURCE13::exti_source() | SOURCE14::exti_source() | SOURCE15::exti_source() |
		SOURCE16::exti_source() | SOURCE17::exti_source();

	static constexpr uint32_t trigger_rising  =
		SOURCE0::exti_trigger_rising() | SOURCE1::exti_trigger_rising() |
		SOURCE2::exti_trigger_rising() | SOURCE3::exti_trigger_rising() |
		SOURCE4::exti_trigger_rising() | SOURCE5::exti_trigger_rising() |
		SOURCE6::exti_trigger_rising() | SOURCE7::exti_trigger_rising() |
		SOURCE8::exti_trigger_rising() | SOURCE9::exti_trigger_rising() |
		SOURCE10::exti_trigger_rising() | SOURCE11::exti_trigger_rising() |
		SOURCE12::exti_trigger_rising() | SOURCE13::exti_trigger_rising() |
		SOURCE14::exti_trigger_rising() | SOURCE15::exti_trigger_rising() |
		SOURCE16::exti_trigger_rising() | SOURCE17::exti_trigger_rising();

	static constexpr uint32_t trigger_falling =
		SOURCE0::exti_trigger_falling() | SOURCE1::exti_trigger_falling() |
		SOURCE2::exti_trigger_falling() | SOURCE3::exti_trigger_falling() |
		SOURCE4::exti_trigger_falling() | SOURCE5::exti_trigger_falling() |
		SOURCE6::exti_trigger_falling() | SOURCE7::exti_trigger_falling() |
		SOURCE8::exti_trigger_falling() | SOURCE9::exti_trigger_falling() |
		SOURCE10::exti_trigger_falling() | SOURCE11::exti_trigger_falling() |
		SOURCE12::exti_trigger_falling() | SOURCE13::exti_trigger_falling() |
		SOURCE14::exti_trigger_falling() | SOURCE15::exti_trigger_falling() |
		SOURCE16::exti_trigger_falling() | SOURCE17::exti_trigger_falling();

	static void init(void) {
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
		EXTI->IMR = interrupt_mask;
		EXTI->RTSR = trigger_rising;
		EXTI->FTSR = trigger_falling;
		NVIC->ISER[0] |=
			(interrupt_mask & 0x0001 ? 1 << EXTI0_IRQn : 0) |
			(interrupt_mask & 0x0002 ? 1 << EXTI1_IRQn : 0) |
			(interrupt_mask & 0x0004 ? 1 << EXTI2_IRQn : 0) |
			(interrupt_mask & 0x0008 ? 1 << EXTI3_IRQn : 0) |
			(interrupt_mask & 0x0010 ? 1 << EXTI4_IRQn : 0) |
			(interrupt_mask & 0x03e0 ? 1 << EXTI9_5_IRQn : 0) |
			(interrupt_mask & 0xfc00 ? 1 << EXTI15_10_IRQn : 0);
		AFIO->EXTICR[0] =
			SOURCE0::exti_cr(0) | SOURCE1::exti_cr(0) | SOURCE2::exti_cr(0) | SOURCE3::exti_cr(0) |
			SOURCE4::exti_cr(0) | SOURCE5::exti_cr(0) | SOURCE6::exti_cr(0) | SOURCE7::exti_cr(0) |
			SOURCE8::exti_cr(0) | SOURCE9::exti_cr(0) | SOURCE10::exti_cr(0) | SOURCE11::exti_cr(0) |
			SOURCE12::exti_cr(0) | SOURCE13::exti_cr(0) | SOURCE14::exti_cr(0) | SOURCE15::exti_cr(0) |
			SOURCE16::exti_cr(0) | SOURCE17::exti_cr(0);
		AFIO->EXTICR[1] =
			SOURCE0::exti_cr(1) | SOURCE1::exti_cr(1) | SOURCE2::exti_cr(1) | SOURCE3::exti_cr(1) |
			SOURCE4::exti_cr(1) | SOURCE5::exti_cr(1) | SOURCE6::exti_cr(1) | SOURCE7::exti_cr(1) |
			SOURCE8::exti_cr(1) | SOURCE9::exti_cr(1) | SOURCE11::exti_cr(1) | SOURCE11::exti_cr(1) |
			SOURCE12::exti_cr(1) | SOURCE13::exti_cr(1) | SOURCE14::exti_cr(1) | SOURCE15::exti_cr(1) |
			SOURCE16::exti_cr(1) | SOURCE17::exti_cr(1);
		AFIO->EXTICR[2] =
			SOURCE0::exti_cr(2) | SOURCE1::exti_cr(2) | SOURCE2::exti_cr(2) | SOURCE3::exti_cr(2) |
			SOURCE4::exti_cr(2) | SOURCE5::exti_cr(2) | SOURCE6::exti_cr(2) | SOURCE7::exti_cr(2) |
			SOURCE8::exti_cr(2) | SOURCE9::exti_cr(2) | SOURCE12::exti_cr(2) | SOURCE11::exti_cr(2) |
			SOURCE12::exti_cr(2) | SOURCE13::exti_cr(2) | SOURCE14::exti_cr(2) | SOURCE15::exti_cr(2) |
			SOURCE16::exti_cr(2) | SOURCE17::exti_cr(2);
		AFIO->EXTICR[3] =
			SOURCE0::exti_cr(3) | SOURCE1::exti_cr(3) | SOURCE2::exti_cr(3) | SOURCE3::exti_cr(3) |
			SOURCE4::exti_cr(3) | SOURCE5::exti_cr(3) | SOURCE6::exti_cr(3) | SOURCE7::exti_cr(3) |
			SOURCE8::exti_cr(3) | SOURCE9::exti_cr(3) | SOURCE13::exti_cr(3) | SOURCE11::exti_cr(3) |
			SOURCE12::exti_cr(3) | SOURCE13::exti_cr(3) | SOURCE14::exti_cr(3) | SOURCE15::exti_cr(3) |
			SOURCE16::exti_cr(3) | SOURCE17::exti_cr(3);
	}

	static void clear_all_irqs(void) {
		EXTI->PR = 0xffff;
		NVIC_ClearPendingIRQ(EXTI0_IRQn);
		NVIC_ClearPendingIRQ(EXTI1_IRQn);
		NVIC_ClearPendingIRQ(EXTI2_IRQn);
		NVIC_ClearPendingIRQ(EXTI3_IRQn);
		NVIC_ClearPendingIRQ(EXTI4_IRQn);
		NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
		NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	}

	template<const int LINE>
	static void handle_irq(void) {
		switch (LINE) {
		case 0: NVIC_ClearPendingIRQ(EXTI0_IRQn); break;
		case 1: NVIC_ClearPendingIRQ(EXTI1_IRQn); break;
		case 2: NVIC_ClearPendingIRQ(EXTI2_IRQn); break;
		case 3: NVIC_ClearPendingIRQ(EXTI3_IRQn); break;
		case 4: NVIC_ClearPendingIRQ(EXTI4_IRQn); break;
		}
		EXTI_IRQ_STATE |= (1 << LINE);
		EXTI->PR = 1 << LINE;
	}

	static constexpr uint32_t irq_line_mask(const int from, const int to) {
		return from <= to ? (1 << from) | irq_line_mask(from + 1, to) : 0;
	}

	template<const int FROM_LINE, const int TO_LINE>
	static void handle_irq(void) {
		const uint32_t line_mask = irq_line_mask(FROM_LINE, TO_LINE);
		if (FROM_LINE == 5) {
			NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
		} else {
			NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
		}
		EXTI_IRQ_STATE |= (EXTI->PR & line_mask);
		EXTI->PR = line_mask;
	}


};
