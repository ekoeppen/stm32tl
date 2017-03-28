#pragma once

template<typename SPI, typename CSN, typename EN>
struct MC33879_T {
	static uint16_t state;

	static void init(void) {
		state = 0;
		EN::set_high();
		for (volatile int i = 0; i < 1000; i++);
	};

	static uint16_t send(uint16_t data) {
		CSN::set_low();
		if (SPI::data_length == 16) {
			state = SPI::transfer(data);
		} else {
			state = SPI::transfer(data >> 8) << 8;
			state |= SPI::transfer(data & 0xff);
		}
		CSN::set_high();
		return state;
	};
};

template<typename SPI, typename CSN, typename EN>
uint16_t MC33879_T<SPI, CSN, EN>::state;
