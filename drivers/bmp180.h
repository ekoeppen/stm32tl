#ifndef __BMP180_H
#define __BMP180_h

template<typename I2C>
struct BMP180_T {
	enum PRESSURE_ACCURACY {
		ULTRA_LOW_POWER,
		STANDARD,
		HIGH_RESOLUTION,
		ULTRA_HIGH_RESOLUTION
	}

	static void init(void) {
	}

	static void disable(void) {
	}

	static int8_t temperature(void) {
	}

	static uint16_t pressure(PRESSURE_ACCURACY accuracy) {
	}
};

#endif
