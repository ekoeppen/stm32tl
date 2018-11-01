#pragma once

template <typename T>
class mem_ptr {
	uint32_t addr;
public:
	constexpr mem_ptr(uint32_t i) : addr{i} {}
	operator T*() const { return reinterpret_cast<T*>(addr); }
	T* operator->() const { return operator T*(); }
};

extern char *itoa_ext(int value, unsigned int radix, unsigned int uppercase, unsigned int value_unsigned, int zero_pad);
extern void delay(int count);

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))
#define ABS_DIFF(a, b) (a > b ? a - b : b - a)
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
