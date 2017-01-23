#pragma once

#include <clocks.h>

template<const uint32_t TOS, const uint32_t STACK_SIZE = 256>
struct TASK_T {
	constexpr static uint32_t stack_top = TOS;
	constexpr static uint32_t stack_bottom = TOS - STACK_SIZE;
	constexpr static uint32_t stack_size = STACK_SIZE;
	constexpr static uint32_t *stack = reinterpret_cast<uint32_t*>(stack_top);

	template<void RUN(void), typename NEXT>
	static void init() {
		/* Save initial context, layout:
		 * [ stack pointer  ]
		 * [ next stack     ]
		 * [ wait semaphore ]
		 * [ pc             ]
		 * [ r0 ... r7      ] */
		asm("mov r11, sp");
		asm("mov r0, %[tos];  mov r1, r0; mov sp, r0" : : [tos] "r" (stack_top));
		asm("mov r0, %[next]; push {r0}"              : : [next] "r" (NEXT::stack_top));
		asm("mov r0, %[wait]; push {r0}"              : : [wait] "r" (0));
		asm("mov r0, %[run];  push {r0}"              : : [run] "r" (RUN));
		asm("push {r0-r7}");
		asm("mov r0, sp; str r0, [r1]");
		asm("mov sp, r11");
	}

	static void activate(void) {
		asm("mov r0, %[tos]; mov r9, r0; ldr r0, [r0]; mov sp, r0" : : [tos] "r" (stack_top));
		asm("pop {r0-r7, pc}");
	}

};

void yield(void) __attribute__((optimize("-fomit-frame-pointer")));
void yield(void) {
	/* Save context */
	asm("push {r0-r7, lr}");
	asm("mov r0, sp; mov r1, r9; str r0, [r1]");

	/* Resume next's context */
	asm("sub r1, r1, #4; ldr r0, [r1]; mov r9, r0; ldr r0, [r0]; mov sp, r0");
	asm("pop {r0-r7, pc}");
}

struct TIMEOUT_NEVER {
	static constexpr bool never = true;
	static constexpr bool immediately = false;
	static void set(const uint32_t milliseconds) { }
	static inline bool count_down(void) { return false; }
	static inline bool triggered(void) { return false; }
	static inline uint32_t get() { return 1; }
	static void disable(void) { }
};

struct TIMEOUT_IMMEDIATELY {
	static constexpr bool never = false;
	static constexpr bool immediately = true;
	static void set(const uint32_t milliseconds) { }
	static inline bool count_down(void) { return true; }
	static inline bool triggered(void) { return true; }
	static inline uint32_t get() { return 0; }
	static void disable(void) { }
};

template<typename CLOCK>
struct TIMEOUT_T {
	static volatile uint32_t timeout;
	static constexpr bool never = false;
	static constexpr bool immediately = false;

	static void set(const uint32_t milliseconds) {
		timeout = milliseconds * CLOCK::frequency / 1000;
		CLOCK::claim();
	};

	static inline bool count_down(void) {
		return !timeout || (--timeout == 0);
	};

	static inline bool triggered(void) {
		return timeout == 0;
	};

	static inline void disable(void) {
		CLOCK::release();
	}

	static inline uint32_t get() {
		return timeout;
	};

	static void set_and_wait(const uint32_t milliseconds) {
		set(milliseconds);
		while (!triggered()) {
			enter_idle();
		}
		disable();
	}

	static void set_and_wait_us(const uint32_t microseconds) {
		CLOCK::set_and_wait_us(microseconds);
	}
};

template<typename CLOCK>
volatile uint32_t TIMEOUT_T<CLOCK>::timeout;
