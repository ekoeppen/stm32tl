#pragma once

#include <stm32f1xx.h>
#include <tasks.h>

extern volatile uint32_t EXTI_IRQ_STATE;

enum GPIO_PORT_ID {
	PA = 0,
	PB = 1,
	PC = 2,
	PD = 3,
	PE = 4,
	PF = 5,
	PG = 6,
	UNUSED_PORT = -1
};

enum GPIO_MODE {
	INPUT = 0,
	OUTPUT_10MHZ = 1,
	OUTPUT_2MHZ = 2,
	OUTPUT_50MHZ = 3
};

enum GPIO_CONF {
	ANALOG = 0,
	FLOATING = 1,
	INPUT_PUSH_PULL = 2,
	PUSH_PULL = 0,
	OPENDRAIN = 1,
	ALT_PUSH_PULL = 2,
	ALT_OPENDRAIN = 3
};

enum EDGE {
	EDGE_RISING,
	EDGE_FALLING,
	EDGE_BOTH
};

enum LEVEL {
	LOW = 0,
	HIGH = 1
};

enum INTERRUPT_ENABLE {
	INTERRUPT_DISABLED = false,
	INTERRUPT_ENABLED = true
};

template<const GPIO_PORT_ID PORT,
      	const uint8_t PIN,
	const GPIO_MODE MODE = INPUT,
	const GPIO_CONF CONF = FLOATING,
	const LEVEL INITIAL_LEVEL = LOW,
	const INTERRUPT_ENABLE INTERRUPT = INTERRUPT_DISABLED,
	const EDGE EXTI_EDGE = EDGE_RISING>
struct GPIO_T {
	static constexpr GPIO_TypeDef *port = ((GPIO_TypeDef *) (GPIOA_BASE + PORT * 0x400));
      	static constexpr uint8_t pin = PIN;
	static constexpr GPIO_MODE mode = MODE;
	static constexpr GPIO_CONF conf = CONF;
	static constexpr uint8_t mode_conf = (CONF << 2 | MODE);
	static constexpr uint32_t mode_conf_low = PIN < 8 ? mode_conf << (PIN << 2) : 0;
	static constexpr uint32_t mode_conf_low_mask = 0xffffffff & ~(PIN < 8 ? 0xf << (PIN << 2) : 0);
	static constexpr uint32_t mode_conf_high = PIN >= 8 ? mode_conf << ((PIN - 8) << 2) : 0;
	static constexpr uint32_t mode_conf_high_mask = 0xffffffff & ~(PIN >= 8 ? 0xf << ((PIN - 8) << 2) : 0);
	static constexpr INTERRUPT_ENABLE interrupt = INTERRUPT;
	static constexpr EDGE exti_edge = EXTI_EDGE;
	static constexpr uint16_t bit_value = 1 << PIN;
	static constexpr uint32_t initial_level = INITIAL_LEVEL << PIN;
	static constexpr uint32_t adc_channel = PIN;
	static constexpr bool is_unused = false;

	static void init(void) {
		if (MODE != INPUT && CONF != FLOATING) {
			if (PIN < 8) {
				port->CRL = (port->CRL & mode_conf_low_mask) | mode_conf_low;
			} else {
				port->CRH = (port->CRL & mode_conf_high_mask) | mode_conf_high;
			}
		}
		if (INITIAL_LEVEL == HIGH) {
			port->BSRR = bit_value;
		}
	}

	static void set_high(void) {
		port->BSRR = bit_value;
	};

	static void set_low(void) {
		port->BRR = bit_value;
	}

	static void set_value(bool value) {
		if (value) set_high();
		else set_low();
	}

	static bool get_value(void) {
		return port->IDR & bit_value;
	}

	static bool is_high(void) {
		return port->IDR & bit_value;
	}

	static bool is_low(void) {
		return !is_high();
	}

	static void toggle(void) {
		if (is_high()) set_low();
		else set_high();
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

	static void set_output(void) {
	}

	static void set_input(void) {
	}

	static void enable_resistor(void) {
	}

	static void disable_resistor(void) {
	}

	static void pull_up(void) {
		port->ODR |= bit_value;
	}

	static void pull_down(void) {
		port->ODR &= ~bit_value;
	}

	static constexpr int exti_source(void) {
		return 1 << PIN;
	}

	static constexpr int exti_trigger_falling(void) {
		return EXTI_EDGE == EDGE_FALLING || EXTI_EDGE == EDGE_BOTH ? 1 << PIN : 0;
	}

	static constexpr int exti_trigger_rising(void) {
		return EXTI_EDGE == EDGE_RISING || EXTI_EDGE == EDGE_BOTH ? 1 << PIN : 0;
	}

	static constexpr int exti_cr(const int i) {
		return PIN / 4 == i ? (PORT << (4 * (PIN % 4))) : 0;
	}
};

template<const GPIO_PORT_ID PORT, const uint8_t PIN>
struct GPIO_OUTPUT_T: public GPIO_T<PORT, PIN, OUTPUT_10MHZ, PUSH_PULL> {
};

template<const GPIO_PORT_ID PORT, const uint8_t PIN>
struct GPIO_INPUT_T: public GPIO_T<PORT, PIN> {
};

struct PIN_UNUSED {
      	static constexpr uint8_t pin = 0;
	static constexpr GPIO_MODE mode = INPUT;
	static constexpr GPIO_CONF conf = FLOATING;
	static constexpr uint32_t mode_conf_low = 0;
	static constexpr uint32_t mode_conf_low_mask = 0xffffffff;
	static constexpr uint32_t mode_conf_high = 0;
	static constexpr uint32_t mode_conf_high_mask = 0xffffffff;
	static constexpr bool af = false;
	static constexpr INTERRUPT_ENABLE interrupt = INTERRUPT_DISABLED;
	static constexpr EDGE edge = EDGE_RISING;
	static constexpr uint16_t bit_value = 0;
	static constexpr uint32_t initial_level = LOW;
	static constexpr uint32_t adc_channel = 0;
	static constexpr bool is_unused = true;

	static constexpr bool get_value(void) { return false; }
	static void set_value(bool) { }
	static constexpr bool is_high(void) { return false; }
	static constexpr bool is_low(void) { return true; }
	static void toggle(void) { }
	static void set_high(void) { }
	static void set_low(void) { }
	static void clear_irq(void) { }
	static void wait_for_irq(void) { }
	static constexpr bool irq_raised(void) { return false; }
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
	static constexpr GPIO_TypeDef *port = ((GPIO_TypeDef *) (GPIOA_BASE + PORT * 0x400));
	static constexpr uint32_t crl = (0x44444444 &
		PIN0::mode_conf_low_mask & PIN1::mode_conf_low_mask & PIN2::mode_conf_low_mask & PIN3::mode_conf_low_mask &
		PIN4::mode_conf_low_mask & PIN5::mode_conf_low_mask & PIN6::mode_conf_low_mask & PIN7::mode_conf_low_mask &
		PIN8::mode_conf_low_mask & PIN9::mode_conf_low_mask & PIN10::mode_conf_low_mask & PIN11::mode_conf_low_mask &
		PIN12::mode_conf_low_mask & PIN13::mode_conf_low_mask & PIN14::mode_conf_low_mask & PIN15::mode_conf_low_mask) |
		PIN0::mode_conf_low | PIN1::mode_conf_low | PIN2::mode_conf_low | PIN3::mode_conf_low |
		PIN4::mode_conf_low | PIN5::mode_conf_low | PIN6::mode_conf_low | PIN7::mode_conf_low |
		PIN8::mode_conf_low | PIN9::mode_conf_low | PIN10::mode_conf_low | PIN11::mode_conf_low |
		PIN12::mode_conf_low | PIN13::mode_conf_low | PIN14::mode_conf_low | PIN15::mode_conf_low;
	static constexpr uint32_t crh = (0x44444444 &
		PIN0::mode_conf_high_mask & PIN1::mode_conf_high_mask & PIN2::mode_conf_high_mask & PIN3::mode_conf_high_mask &
		PIN4::mode_conf_high_mask & PIN5::mode_conf_high_mask & PIN6::mode_conf_high_mask & PIN7::mode_conf_high_mask &
		PIN8::mode_conf_high_mask & PIN9::mode_conf_high_mask & PIN10::mode_conf_high_mask & PIN11::mode_conf_high_mask &
		PIN12::mode_conf_high_mask & PIN13::mode_conf_high_mask & PIN14::mode_conf_high_mask & PIN15::mode_conf_high_mask) |
		PIN0::mode_conf_high | PIN1::mode_conf_high | PIN2::mode_conf_high | PIN3::mode_conf_high |
		PIN4::mode_conf_high | PIN5::mode_conf_high | PIN6::mode_conf_high | PIN7::mode_conf_high |
		PIN8::mode_conf_high | PIN9::mode_conf_high | PIN10::mode_conf_high | PIN11::mode_conf_high |
		PIN12::mode_conf_high | PIN13::mode_conf_high | PIN14::mode_conf_high | PIN15::mode_conf_high;
	static constexpr uint32_t odr =
		PIN0::initial_level | PIN1::initial_level | PIN2::initial_level | PIN3::initial_level |
		PIN4::initial_level | PIN5::initial_level | PIN6::initial_level | PIN7::initial_level |
		PIN8::initial_level | PIN9::initial_level | PIN10::initial_level | PIN11::initial_level |
		PIN12::initial_level | PIN13::initial_level | PIN14::initial_level | PIN15::initial_level;

	static void init(void) {
		RCC->APB2ENR |= (1 << (PORT + 2));
		if (crl != 0x44444444) port->CRL = crl;
		if (crh != 0x44444444) port->CRH = crh;
		port->ODR = odr;
	}

	static void disable(void) {
		RCC->APB2ENR |= (1 << (PORT + 2));
		port->CRL = 0x44444444;
		port->CRH = 0x44444444;
		RCC->APB2ENR &= ~(1 << (PORT + 2));
	}
};
