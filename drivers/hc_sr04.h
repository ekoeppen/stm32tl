#pragma once

#include <utils.h>
#include <limits.h>

template<typename COUNTER, typename TRIGGER, typename ECHO>
struct HC_SR04 {
	static void init(void) {
		COUNTER::set_frequency(17182);
		TRIGGER::set_low();
	}

	static uint32_t measure(void) {
		TRIGGER::set_high();
		for (volatile int i = 0; i < 10000; i++);
		TRIGGER::set_low();

		while (ECHO::is_low());
		COUNTER::start();
		while (ECHO::is_high());
		COUNTER::stop();
		return COUNTER::overflow() ? INT_MAX : COUNTER::value();
	}
};
