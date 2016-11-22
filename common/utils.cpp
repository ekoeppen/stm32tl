#include <stdint.h>
#include <utils.h>

char *itoa_ext(int value, unsigned int radix, unsigned int uppercase, unsigned int value_unsigned, int zero_pad)
{
	static char buffer[11];
	char *p = &buffer[10];
	int negative = 0;
	int digit;

	if (value < 0 && radix == 10 && !value_unsigned) {
		negative = 1;
		value = -value;
	}

	*p = '\0';
	do {
		p--;
		digit = value % radix;
		*p = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
		value /= radix;
		zero_pad--;
	} while (value > 0);

	while (zero_pad > 0) {
		p--;
		*p = '0';
		zero_pad--;
	}
	if (negative) {
		p--;
		*p = '-';
	}

	return p;
}

void delay(int count) {
	while (count--) {
		__asm("nop");
	}
}
