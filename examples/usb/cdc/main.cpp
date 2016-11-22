#include <string.h>
#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <stddef.h>
#include <io.h>

#include <drivers/ringbuffer.h>
#include <usb/usb.h>
#include <drivers/usb_cdc.h>

#include <board.h>

#ifdef STM32F0xx
typedef SYSCLK_T<HSI48_RC> SYSCLK;
#else
typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
#endif
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON | BOARD_T::CONSOLE> BOARD;

typedef GPIO_OUTPUT_T<PA, 4> PIN;

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}

}

char buffer[96];

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
	PIN::init();

#ifdef INTERACTIVE
	CON::getc();
	LED1::set_high();
	while (1) {
		LED2::toggle();
		CON::puts("Press any key... \n");
		char c = CON::getc();
		printf<CON>("Key pressed: %c %02x\n", c, c);

		CON::puts("Enter a string...\n");
		buffer[CON::gets(buffer, sizeof(buffer) - 1)] = '\0';
		CON::puts(buffer);
	}
#else
	while (1) {
		LED2::toggle();
		uint32_t n = CON::read((uint8_t *) buffer, sizeof(buffer));
		hex_dump<CON>(buffer, n, 0, true);
	}
#endif
	return 0;
}
