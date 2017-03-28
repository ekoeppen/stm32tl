#ifndef __MMC_H
#define __MMC_H

template<typename SPI,
	typename CS>
struct MMC_T {
	static void init(void) {
		CS::set_high();
		for (int i = 0; i < 10; i++) {
			SPI::transfer(0xff);
		}
		CS::set_low();
		SPI::transfer((uint8_t *) "\x40\x00\x00\x00\x00\x95", 6, 0);
		for (int i = 0; i < 64; i++) {
			uint8_t r = SPI::transfer(0xff);
			if (r != 0xff) break;
		}
	};
};

#endif
