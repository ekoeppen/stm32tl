#ifndef __CIRCULAR_LOG_H
#define __CIRCULAR_LOG_H

template<typename DATA,
	typename STORAGE,
	const uint32_t SIZE>
struct CIRCULAR_LOG_T {
	static void reset(void) {
		uint32_t address = sizeof(address);
		STORAGE::write(0, (uint8_t *) &address, sizeof(address));
	}

	static void append(DATA& data) {
		uint32_t address;
		STORAGE::read(0, (uint8_t *) &address, sizeof(address));
		address += sizeof(data);
		if (address >= SIZE) address = sizeof(address);
		STORAGE::write(0, (uint8_t *) &address, sizeof(address));
		STORAGE::write(address, (uint8_t *) &data, sizeof(data));
	}

	template<typename UART>
	static void dump(void) {
		uint32_t address;
		DATA data;

		UART::puts("---- Data log -----------------------------------------------------\n");
		STORAGE::read(0, (uint8_t *) &address, sizeof(address));
		printf<UART>("Last address: %d\n", address);
		for (uint32_t i = sizeof(address); i < SIZE; i += sizeof(data)) {
			STORAGE::read(i, (uint8_t *) &data, sizeof(data));
			printf<UART>("[%05d]: ", i);
			data.template print<UART>();
			UART::putc('\n');
		}
	}
};

#endif
