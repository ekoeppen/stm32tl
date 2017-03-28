#pragma once

template<typename CLOCK, typename LED>
struct BLINK_T {
	static uint32_t timeout;
	static uint32_t count;
	static uint32_t duration;

	static void blink(uint32_t blink_count, uint32_t blink_duration) {
		count = blink_count * 2 - 1;
		duration = blink_duration;
		timeout = duration * CLOCK::frequency / 1000;
		LED::set_high();
	}

	static void off(void) {
		count = 0;
		LED::set_low();
	}

	static void handle_irq(void) {
		if (count > 0) {
			timeout--;
			if (timeout == 0) {
				LED::toggle();
				count--;
				timeout = duration * CLOCK::frequency / 1000;
			}
		}
	}
};
template<typename CLOCK, typename LED>
uint32_t BLINK_T<CLOCK, LED>::timeout;

template<typename CLOCK, typename LED>
uint32_t BLINK_T<CLOCK, LED>::count;

template<typename CLOCK, typename LED>
uint32_t BLINK_T<CLOCK, LED>::duration;


