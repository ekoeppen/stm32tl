#pragma once

#include <io.h>

template<typename OUTPUT, typename DATA>
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

void memcpy(void *dst, const void *src, int len);
void memset(void *dst, uint8_t c, int len);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))
#define ABS_DIFF(a, b) (a > b ? a - b : b - a)
#define hex_dump_periph(OUTPUT, PERIPH) hex_dump<OUTPUT, uint32_t>((uint32_t *) PERIPH, sizeof(*PERIPH), #PERIPH)
#define FROM_HEX(c) (c >= '0' && c <= '9' ? c - '0' : (c >= 'A' && c <= 'F' ? c - 'A' + 10 : (c >= 'a' && c <= 'f' ? c - 'a' + 10 : 0)))
#define HEX_DIGIT(n) ((n) + ((n) < 10 ? '0' : 'A' - 10))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BIT0                   (0x0001)
#define BIT1                   (0x0002)
#define BIT2                   (0x0004)
#define BIT3                   (0x0008)
#define BIT4                   (0x0010)
#define BIT5                   (0x0020)
#define BIT6                   (0x0040)
#define BIT7                   (0x0080)
#define BIT8                   (0x0100)
#define BIT9                   (0x0200)
#define BIT10                  (0x0400)
#define BIT11                  (0x0800)
#define BIT12                  (0x1000)
#define BIT13                  (0x2000)
#define BIT14                  (0x4000)
#define BIT15                  (0x8000)
#define BITA                   (0x0400)
#define BITB                   (0x0800)
#define BITC                   (0x1000)
#define BITD                   (0x2000)
#define BITE                   (0x4000)
#define BITF                   (0x8000)
