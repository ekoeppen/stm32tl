#pragma once

#include <clocks.h>
#include <tasks.h>
#include <utils.h>
#include <stdarg.h>

extern char *itoa_ext(int value, unsigned int radix, unsigned int uppercase, unsigned int value_unsigned, int zero_pad);
template<class T, class U>
struct is_same {
	enum { value = 0 };
};

template<class T>
struct is_same<T, T> {
	enum { value = 1 };
};

struct DISABLED_UART {
	template<typename TIMEOUT = TIMEOUT_NEVER> static void init(void) { }
	template<typename TIMEOUT = TIMEOUT_NEVER> static void putc(char data) { }
	template<typename TIMEOUT = TIMEOUT_NEVER> static void puts(const char *data) { }
	template<typename TIMEOUT = TIMEOUT_NEVER> static char getc() { return 0; }
	static bool handle_irq(void) { return true; }
	static constexpr bool enabled(void) { return false; }
	static void enable(void) { }
	static void disable(void) { }
};

template<typename OUTPUT, typename TIMEOUT = TIMEOUT_NEVER>
void vprintf(const char *fmt, va_list va)
{
	char ch;

	while ((ch = *fmt++) != 0) {
		if (ch != '%') {
			OUTPUT::template putc<TIMEOUT>(ch);
		} else {
			char zero_pad = 0;

			ch = *fmt++;

			/* Zero padding requested */
			if (ch == '0') {
				ch = *fmt++;
				if (ch >= '0' && ch <= '9')
					zero_pad = ch - '0';
				ch = *fmt++;
			}

			switch (ch) {
				case 'u':
				case 'd':
				case 'x':
				case 'X':
					OUTPUT::template puts<TIMEOUT>(itoa_ext(va_arg(va, unsigned int),
								ch == 'u' || ch == 'd' ? 10 : 16,
								ch == 'X', ch == 'u', zero_pad));
					break;
				case 'c' :
					OUTPUT::putc((char)(va_arg(va, int)));
					break;
				case 's' :
					OUTPUT::template puts<TIMEOUT>(va_arg(va, char*));
					break;
				default:
					OUTPUT::template putc<TIMEOUT>(ch);
					break;
			}
		}
	}
}


template<typename OUTPUT, typename TIMEOUT = TIMEOUT_NEVER>
void printf(const char *fmt, ...)
{
	if (OUTPUT::enabled()) {
		va_list va;

		va_start(va, fmt);
		vprintf<OUTPUT, TIMEOUT>(fmt, va);
		va_end(va);
	}
}

template<typename INPUT, typename TIMEOUT = TIMEOUT_NEVER>
void gets(char *buffer, int count)
{
	while (count > 0) {
		*buffer = INPUT::template getc<TIMEOUT>();
		buffer++;
		count--;
	}
}

template<typename OUTPUT, typename DATA = uint8_t>
void hex_dump(DATA *start, int count, const char *title = 0, bool relative = false)
{
	DATA *n = 0;
	if (title) printf<OUTPUT>("%s:\n", title);
	while (count > 0) {
		printf<OUTPUT>(sizeof(DATA) == 1 ? "%04x: %02x %02x %02x %02x\n" :
			      (sizeof(DATA) == 2 ? "%04x: %04x %04x %04x %04x\n" : "%08x: %08x %08x %08x %08x\n"),
			       relative ? n : start, start[0], start[1], start[2], start[3]);
		start += sizeof(DATA) * 4;
		count -= sizeof(DATA) * 4;
		n += sizeof(DATA) * 4;
	}
}

#define hex_dump_periph(OUTPUT, PERIPH) hex_dump<OUTPUT, uint32_t>((uint32_t *) PERIPH, sizeof(*PERIPH), #PERIPH)
