#pragma once

#include <limits.h>

namespace RINGBUFFER_T {

template<const uint32_t ID,
	typename DATA,
	const uint32_t SIZE,
	const bool CHECKED = true>
struct BUFFER {
	static DATA buffer[SIZE];
	static volatile uint32_t head;
	static volatile uint32_t tail;

	static void init(void) {
		head = tail = 0;
	}

	static bool is_full(void) {
		return CHECKED ? (tail + 1) % SIZE == head : false;
	}

	static bool is_empty(void) {
		return CHECKED ? tail == head : true;
	}

	static uint32_t free_space(void) {
		return head >= tail ? SIZE - (head - tail) : tail - head;
	}

	static uint32_t used_space(void) {
		return SIZE - free_space();
	}

	static void write(DATA c) {
		head = (head + 1) % SIZE;
		buffer[head] = c;
	}

	static DATA read() {
		tail = (tail + 1) % SIZE;
		return buffer[tail];
	}

};

template<const uint32_t ID, typename DATA, const uint32_t SIZE, const bool CHECKED>
DATA BUFFER<ID, DATA, SIZE, CHECKED>::buffer[SIZE];

template<const uint32_t ID, typename DATA, const uint32_t SIZE, const bool CHECKED>
volatile uint32_t BUFFER<ID, DATA, SIZE, CHECKED>::head;

template<const uint32_t ID, typename DATA, const uint32_t SIZE, const bool CHECKED>
volatile uint32_t BUFFER<ID, DATA, SIZE, CHECKED>::tail;

template<typename DATA>
struct NULL_BUFFER {
	static void init(void) { }
	static bool is_full(void) { return false; }
	static bool is_empty(void) { return true; }
	static uint32_t free_space(void) { return UINT_MAX; }
	static uint32_t used_space(void) { return 0; }
	static void write(DATA) { }
	static DATA read() { return (DATA) 0; }
};

}
