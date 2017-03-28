#pragma once

template<typename SPI, typename SHLD>
struct LV165A {
	static void init() {
	}

	static uint8_t read(void) {
		uint8_t r;
		SHLD::set_high();
		r = SPI::transfer(0xff);
		SHLD::set_low();
		return r;
	}
};
