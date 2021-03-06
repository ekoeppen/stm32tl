#ifndef LSM6DS3_H
#define LSM6DS3_H

namespace LSM6DS3 {

template<typename SPI, typename CSN>
struct T {
	static void init(void) {
	}

	static void disable(void) {
	}

	static void write_reg(uint8_t reg, uint8_t value) {
		CSN::set_low();
		SPI::transfer(reg);
		SPI::transfer(value);
		CSN::set_high();
	}

	static uint8_t read_reg(uint8_t reg) {
		uint8_t value;
		CSN::set_low();
		SPI::transfer(0x80 | reg);
		value = SPI::transfer(0x0ff);
		CSN::set_high();
		return value;
	}

	static void read_regs(uint8_t *buffer) {
		for (int i = 0; i < 108; i++) {
			buffer[i] = read_reg(i);
		}
	}
};

}

#endif
