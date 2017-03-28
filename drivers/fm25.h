#ifndef __FM25_H
#define __FM25_H

template<typename SPI,
	typename CS,
	typename ADDR_T = uint16_t>
struct FM25_T {
	static const uint8_t WREN = 0b00000110;
	static const uint8_t WRDI = 0b00000100;
	static const uint8_t RDSR = 0b00000101;
	static const uint8_t WRSR = 0b00000001;
	static const uint8_t READ = 0b00000011;
	static const uint8_t WRITE = 0b00000010;

	static uint8_t init(void) {
		return read_status();
	}

	static uint8_t read_status(void) {
		uint8_t r;
		CS::set_low();
		SPI::transfer(RDSR);
		r = SPI::transfer(0xff);
		CS::set_high();
		return r;
	}

	static void erase(void) { }
	static void erase_sector(uint32_t addr) { }

	static void enable_write(void) {
		CS::set_low();
		SPI::transfer(WREN);
		CS::set_high();
	}

	static void disable_write(void) {
		CS::set_low();
		SPI::transfer(WRDI);
		CS::set_high();
	}
	static void read(ADDR_T addr, uint8_t *data, uint16_t count) {
		CS::set_low();
		SPI::transfer(READ);
		SPI::transfer((addr & 0xff00) >> 8);
		SPI::transfer(addr & 0x00ff);
		SPI::transfer(0, count, data);
		CS::set_high();
	}

	static void write(ADDR_T addr, uint8_t *data, uint16_t count) {
		CS::set_low();
		enable_write();
		SPI::transfer(WRITE);
		SPI::transfer((addr & 0xff00) >> 8);
		SPI::transfer(addr & 0x00ff);
		SPI::transfer(data, count);
		CS::set_high();
	}
};

#endif
