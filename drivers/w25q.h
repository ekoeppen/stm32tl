#pragma once

template<typename SPI,
	typename CS>
struct W25Q_T {
	static const uint8_t WRITE_ENABLE = 0x06;
	static const uint8_t WRITE_DISABLE = 0x04;
	static const uint8_t READ_STATUS_REGISTER_1 = 0x05; // (S7–S0) (2)
	static const uint8_t READ_STATUS_REGISTER_2 = 0x35; // (S15-S8) (2)
	static const uint8_t WRITE_STATUS_REGISTER = 0x01; // (S7–S0) (S15-S8)
	static const uint8_t PAGE_PROGRAM = 0x02; // A23–A16 A15–A8 A7–A0 (D7–D0)
	static const uint8_t READ_DATA = 0x03; // A23–A16 A15–A8 A7–A0 (D7–D0)
	static const uint8_t QUAD_PAGE_PROGRAM = 0x32; // A23–A16 A15–A8 A7–A0 (D7–D0, ...)(3)
	static const uint8_t SECTOR_ERASE_4KB = 0x20; // A23–A16 A15–A8 A7–A0
	static const uint8_t BLOCK_ERASE_32KB = 0x52; // A23–A16 A15–A8 A7–A0
	static const uint8_t BLOCK_ERASE_64KB = 0xD8; // A23–A16 A15–A8 A7–A0
	static const uint8_t CHIP_ERASE = 0xC7; // /60
	static const uint8_t ERASE_SUSPEND = 0x75;
	static const uint8_t ERASE_RESUME = 0x7A;
	static const uint8_t POWER_DOWN = 0xB9;
	static const uint8_t CONTINUOUS_READ_MODE_RESET = 0xFF; // FF(4)

	static const uint8_t STATUS_BUSY = 0b00000001;

	static uint8_t init(void) {
		return read_status();
	}

	static uint8_t read_status(void) {
		uint8_t r;
		CS::set_low();
		SPI::transfer(READ_STATUS_REGISTER_1);
		r = SPI::transfer(0xff);
		CS::set_high();
		return r;
	}

	static void enable_write(void) {
		CS::set_low();
		SPI::transfer(WRITE_ENABLE);
		CS::set_high();
	}

	static void erase(void) {
		CS::set_low();
		SPI::transfer(CHIP_ERASE);
		CS::set_high();
		while (read_status() & STATUS_BUSY);
	}

	static void erase_sector(uint32_t addr) {
		CS::set_low();
		SPI::transfer(SECTOR_ERASE_4KB);
		SPI::transfer((addr & 0xff0000) >> 16);
		SPI::transfer((addr & 0x00ff00) >> 8);
		SPI::transfer(addr & 0x0000ff);
		CS::set_high();
		while (read_status() & STATUS_BUSY);
	}

	static void disable_write(void) {
		CS::set_low();
		SPI::transfer(WRITE_DISABLE);
		CS::set_high();
	}
	static void read(uint32_t addr, uint8_t *data, uint16_t count) {
		CS::set_low();
		SPI::transfer(READ_DATA);
		SPI::transfer((addr & 0xff0000) >> 16);
		SPI::transfer((addr & 0x00ff00) >> 8);
		SPI::transfer(addr & 0x0000ff);
		SPI::transfer(0, count, data);
		CS::set_high();
	}

	static void write(uint32_t addr, uint8_t *data, uint16_t count) {
		enable_write();
		CS::set_low();
		SPI::transfer(PAGE_PROGRAM);
		SPI::transfer((addr & 0xff0000) >> 16);
		SPI::transfer((addr & 0x00ff00) >> 8);
		SPI::transfer(addr & 0x0000ff);
		SPI::transfer(data, count);
		CS::set_high();
		while (read_status() & STATUS_BUSY);
	}
};

