#include <string.h>
#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <stddef.h>
#include <io.h>
#include <spi.h>

#include <drivers/ringbuffer.h>
#include <usb/usb.h>
#include <drivers/usb_cdc.h>
#include <drivers/w25q.h>
#include <drivers/cy15b.h>
#include <drivers/fm25.h>
#include <drivers/m25aa.h>

#include <board.h>

#define FLASH_TYPE_CY15
// #define FLASH_TYPE_FM25
// #define FLASH_TYPE_W25
// #define FLASH_TYPE_M25AA

#ifdef STM32F0xx
typedef SYSCLK_T<HSI48_RC> SYSCLK;
#else
typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
#endif
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

typedef GPIO_OUTPUT_T<PA, 4> CSN;
#ifdef STM32F0xx
typedef SPI_T<SYSCLK, true, 0, 8000000> SPI;
#else
typedef SPI_T<SYSCLK, SPI_1, true, 0, 8000000> SPI;
#endif
typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON | BOARD_T::CONSOLE | BOARD_T::SPI> BOARD;

#ifdef FLASH_TYPE_FM25
typedef FM25_T<SPI, CSN> SPI_FLASH;
#elif defined FLASH_TYPE_W25
typedef W25Q_T<SPI, CSN> SPI_FLASH;
#elif defined FLASH_TYPE_CY15
typedef CY15B_T<SPI, CSN> SPI_FLASH;
#elif defined FLASH_TYPE_M25AA
typedef M25AA_T<SPI, CSN> SPI_FLASH;
#endif

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}

}

uint8_t buffer[256];

int main(void)
{
#ifdef STM32F0xx
	HSI48_RC::init();
#else
	HSE::init();
	PLL::init();
#endif
	SYSCLK::init();
	SYSTICK::init();
	BOARD::init();
	CON::init();
	CSN::init();
	CSN::set_high();
	SPI::init();
	SPI_FLASH::init();
	SPI_FLASH::enable_write();
	while (1) {
		char c = CON::getc();
		switch (c) {
		case 's':
			printf<CON>("Status: %02x\n", SPI_FLASH::read_status());
			break;
		case 'e':
			CON::puts("Erasing...\n");
			SPI_FLASH::erase();
			break;
		case 'r':
			for (uint32_t addr = 0; addr < 2 * 1024; addr += 256) {
				SPI_FLASH::read(addr, buffer, sizeof(buffer));
				printf<CON>("%04x\n", addr);
				hex_dump<CON>(buffer, sizeof(buffer), 0, true);
			}
			break;
		case 'd':
			for (uint32_t addr = 0; addr < 2 * 1024; addr += 256) {
				SPI_FLASH::read(addr, buffer, sizeof(buffer));
				CON::write(buffer, sizeof(buffer));
			}
			break;
		case 'D':
			for (uint32_t addr = 0; addr < 64 * 1024; addr += 256) {
				SPI_FLASH::read(addr, buffer, sizeof(buffer));
				CON::write(buffer, sizeof(buffer));
			}
			break;
		case 't':
			CON::puts("Writing test data...\n");
			for (size_t i = 0; i < 256; i++) buffer[i] = (i % 64) + 32;
			buffer[1] = ' ';
			for (uint32_t addr = 0; addr < 2 * 1024; addr += 32) {
				printf<CON>("Addr: %08x...\n", addr);
				buffer[0] = (addr >> 8) + '0';
				SPI_FLASH::write(addr, buffer, sizeof(buffer));
			}
			break;
		case 'w':
			LED1::set_high();
			for (uint32_t addr = 0; addr < 2 * 1024 * 1024; addr += 256) {
				if (addr % 4096 == 0) {
					// SPI_FLASH::erase_sector(addr);
				}
				for (size_t i = 0; i < 256; i += 32) {
					TIMEOUT::set(100);
					CON::read<TIMEOUT>(&buffer[i], 32);
					if (TIMEOUT::triggered()) {
						break;
					}
					TIMEOUT::disable();
				}
				if (!TIMEOUT::triggered()) {
					SPI_FLASH::write(addr, buffer, sizeof(buffer));
				} else {
					break;
				}
			}
			LED1::set_low();
			break;
		default:
			CON::puts("?\n");
			break;
		}
		CON::puts("ok\n");
	}
	return 0;
}
