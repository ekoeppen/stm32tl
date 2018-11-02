#pragma once

#include <stdlib.h>
#include <tasks.h>
#include <utils.h>
#include <stm32f0xx.h>

extern volatile uint32_t EXTI_IRQ_STATE;

enum GPIO_PORT_ID {
	PA = 0,
	PB = 1,
	PC = 2,
	PD = 3,
	PF = 5,
	UNUSED_PORT = -1
};

enum GPIO_MODE {
	MODE_INPUT = 0,
	MODE_OUTPUT = 1,
	MODE_ALTFUNC = 2,
	MODE_ANALOG = 3,
	MODE_MASK = 3
};

enum GPIO_TYPE {
	TYPE_PUSH_PULL = 0,
	TYPE_OPENDRAIN = 1,
	TYPE_MASK = 1
};

enum GPIO_SPEED {
	SPEED_LOW = 0,
	SPEED_MEDIUM = 1,
	SPEED_HIGH = 3,
	SPEED_MASK = 3
};

enum GPIO_PULL {
	PULL_NONE = 0,
	PULL_UP = 1,
	PULL_DOWN = 2,
	PULL_MASK = 3
};

enum GPIO_AF {
	AF0 = 0,
	AF1 = 1,
	AF2 = 2,
	AF3 = 3,
	AF4 = 4,
	AF5 = 5,
	AF6 = 6,
	AF7 = 7,
	AF_MASK = 7
};

enum EDGE {
	EDGE_RISING,
	EDGE_FALLING,
	EDGE_BOTH
};

template<const GPIO_PORT_ID PORT,
	const int PIN,
	const GPIO_MODE MODE = MODE_INPUT,
	const GPIO_TYPE TYPE = TYPE_PUSH_PULL,
	const GPIO_SPEED SPEED = SPEED_LOW,
	const GPIO_PULL PULL = PULL_NONE,
	const GPIO_AF AF = AF0,
	const EDGE EXTI_EDGE = EDGE_RISING>
struct GPIO_T {
	static constexpr auto port = mem_ptr<GPIO_TypeDef>(GPIOA_BASE + PORT * 0x400);

	static void init(void) {
		if (MODE != MODE_INPUT) set_mode(MODE);
		if (TYPE != TYPE_PUSH_PULL) set_type(TYPE);
		if (SPEED != SPEED_LOW) set_speed(SPEED);
		if (PULL != PULL_NONE) set_pull(PULL);
		RCC->AHBENR |= (1 << (PORT + 17));
	}

	static constexpr uint16_t bit_value = 1 << PIN;
	static constexpr uint32_t mode = MODE << (PIN * 2);
	static constexpr uint32_t mode_mask = MODE_MASK << (PIN * 2);
	static constexpr uint32_t type = TYPE << PIN;
	static constexpr uint32_t speed = SPEED << (PIN * 2);
	static constexpr uint32_t pull = PULL << (PIN * 2);
	static constexpr uint32_t altfunc_low = (PIN < 8 ? (AF << (PIN * 4)) : 0);
	static constexpr uint32_t altfunc_low_mask = ~(PIN < 8 ? (AF_MASK << (PIN * 4)) : 0);
	static constexpr uint32_t altfunc_high = (PIN >= 8 ? (AF << ((PIN - 8) * 4)) : 0);
	static constexpr uint32_t altfunc_high_mask = ~(PIN >= 8 ? (AF_MASK << ((PIN - 8) * 4)) : 0);

	static constexpr uint32_t adc_channel = PIN;
	static constexpr uint32_t adc_channel_bit = 1 << PIN;

	static constexpr bool is_unused = false;

	static void set_mode(int mode) {
		port->MODER = (port->MODER & ~(MODE_MASK << (PIN * 2))) | (mode << (PIN * 2));
	}

	static void set_type(int type) {
		port->OTYPER = (port->OTYPER & ~(TYPE_MASK << PIN)) | (type << PIN);
	}

	static void set_speed(int speed) {
		port->OSPEEDR = (port->OSPEEDR & ~(SPEED_MASK << (PIN * 2))) | (speed << (PIN * 2));
	}

	static void set_pull(int pull) {
		port->PUPDR = (port->PUPDR & ~(PULL_MASK << (PIN * 2))) | (pull << (PIN * 2));
	}

	static void set_altfunc(int func) {
		if (PIN < 8) {
			port->AFR[0] = (port->AFR[0] & altfunc_low_mask) | altfunc_low;
		} else {
			port->AFR[1] = (port->AFR[1] & altfunc_high_mask) | altfunc_high;
		}
	}

	static bool get_value(void) {
		return port->IDR & bit_value;
	}

	static bool is_high(void) {
		return get_value();
	}

	static bool is_low(void) {
		return !get_value();
	}

	static void set_value(const int value) {
		port->BSRR = (1 << (PIN + (1 - value) * 16));
	}
	static void set_high(void) {
		port->BSRR = bit_value;
	}
	static void set_low(void) {
		port->BRR = bit_value;
	}

	static void toggle(void) {
		if (port->ODR & bit_value) set_low(); else set_high();
	}

	static void clear_irq(void) {
		EXTI_IRQ_STATE &= ~bit_value;
	}

	static bool irq_raised(void) {
		return EXTI_IRQ_STATE & bit_value;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static bool wait_for_irq(void) {
		while (!irq_raised() && !TIMEOUT::triggered()) {
			enter_idle();
		}
		return irq_raised();
	}

	static constexpr uint32_t exti_source(void) {
		return 1 << PIN;
	}

	static void disable_irq(void) {
		EXTI->IMR &= ~exti_source();
	}

	static void enable_irq(void) {
		EXTI->IMR |= exti_source();
	}

	static constexpr int exti_trigger_falling(void) {
		return EXTI_EDGE == EDGE_FALLING || EXTI_EDGE == EDGE_BOTH ? bit_value : 0;
	}

	static constexpr int exti_trigger_rising(void) {
		return EXTI_EDGE == EDGE_RISING || EXTI_EDGE == EDGE_BOTH ? bit_value : 0;
	}

	static constexpr int exti_syscfg(const int i) {
		return PIN / 4 == i ? (PORT << (4 * (PIN % 4))) : 0;
	}
};

template<const GPIO_PORT_ID PORT, const uint8_t PIN>
struct GPIO_OUTPUT_T: public GPIO_T<PORT, PIN, MODE_OUTPUT> {
};

template<const GPIO_PORT_ID PORT, const uint8_t PIN>
struct GPIO_INPUT_T: public GPIO_T<PORT, PIN> {
};

struct PIN_UNUSED {
	static constexpr uint16_t bit_value = 0;
	static constexpr uint32_t mode = 0;
	static constexpr uint32_t mode_mask = 0;
	static constexpr uint32_t type = 0;
	static constexpr uint32_t speed = 0;
	static constexpr uint32_t pull = 0;
	static constexpr uint32_t altfunc_low = 0;
	static constexpr uint32_t altfunc_high = 0;
	static constexpr bool is_unused = true;
	static constexpr bool is_high(void) { return false; }
	static constexpr bool is_low(void) { return true; }

	static constexpr bool get_value(void) { return false; }
	static void set_value(bool) { }
	static void toggle(void) { }
	static void set_high(void) { }
	static void set_low(void) { }
	static void clear_irq(void) { }
	static void wait_for_irq(void) { }
	static constexpr bool irq_raised(void) { return false; }

	static constexpr uint32_t adc_channel = 0;
	static constexpr uint32_t adc_channel_bit = 0;
};

template <const GPIO_PORT_ID PORT,
	typename PIN0 = PIN_UNUSED,
	typename PIN1 = PIN_UNUSED,
	typename PIN2 = PIN_UNUSED,
	typename PIN3 = PIN_UNUSED,
	typename PIN4 = PIN_UNUSED,
	typename PIN5 = PIN_UNUSED,
	typename PIN6 = PIN_UNUSED,
	typename PIN7 = PIN_UNUSED,
	typename PIN8 = PIN_UNUSED,
	typename PIN9 = PIN_UNUSED,
	typename PIN10 = PIN_UNUSED,
	typename PIN11 = PIN_UNUSED,
	typename PIN12 = PIN_UNUSED,
	typename PIN13 = PIN_UNUSED,
	typename PIN14 = PIN_UNUSED,
	typename PIN15 = PIN_UNUSED>
struct GPIO_PORT_T {
	static constexpr auto port = mem_ptr<GPIO_TypeDef>(GPIOA_BASE + PORT * 0x400);

	static void init(void) {
		RCC->AHBENR |= (1 << (PORT + 17));

		port->MODER =
			PIN0::mode + PIN1::mode + PIN2::mode + PIN3::mode +
			PIN4::mode + PIN5::mode + PIN6::mode + PIN7::mode +
			PIN8::mode + PIN9::mode + PIN10::mode + PIN11::mode +
			PIN12::mode + PIN13::mode + PIN14::mode + PIN15::mode;

		port->OTYPER =
			PIN0::type + PIN1::type + PIN2::type + PIN3::type +
			PIN4::type + PIN5::type + PIN6::type + PIN7::type +
			PIN8::type + PIN9::type + PIN10::type + PIN11::type +
			PIN12::type + PIN13::type + PIN14::type + PIN15::type;

		port->OSPEEDR =
			PIN0::speed + PIN1::speed + PIN2::speed + PIN3::speed +
			PIN4::speed + PIN5::speed + PIN6::speed + PIN7::speed +
			PIN8::speed + PIN9::speed + PIN10::speed + PIN11::speed +
			PIN12::speed + PIN13::speed + PIN14::speed + PIN15::speed;

		port->PUPDR =
			PIN0::pull + PIN1::pull + PIN2::pull + PIN3::pull +
			PIN4::pull + PIN5::pull + PIN6::pull + PIN7::pull +
			PIN8::pull + PIN9::pull + PIN10::pull + PIN11::pull +
			PIN12::pull + PIN13::pull + PIN14::pull + PIN15::pull;

		port->AFR[0] =
			PIN0::altfunc_low + PIN1::altfunc_low + PIN2::altfunc_low + PIN3::altfunc_low +
			PIN4::altfunc_low + PIN5::altfunc_low + PIN6::altfunc_low + PIN7::altfunc_low +
			PIN8::altfunc_low + PIN9::altfunc_low + PIN10::altfunc_low + PIN11::altfunc_low +
			PIN12::altfunc_low + PIN13::altfunc_low + PIN14::altfunc_low + PIN15::altfunc_low;

		port->AFR[1] =
			PIN0::altfunc_high + PIN1::altfunc_high + PIN2::altfunc_high + PIN3::altfunc_high +
			PIN4::altfunc_high + PIN5::altfunc_high + PIN6::altfunc_high + PIN7::altfunc_high +
			PIN8::altfunc_high + PIN9::altfunc_high + PIN10::altfunc_high + PIN11::altfunc_high +
			PIN12::altfunc_high + PIN13::altfunc_high + PIN14::altfunc_high + PIN15::altfunc_high;
	}

	static void disable(void) {
		RCC->AHBENR |= (1 << (PORT + 17));
		port->MODER = 0xffffffff;
		RCC->AHBENR &= ~(1 << (PORT + 17));
	}
};
