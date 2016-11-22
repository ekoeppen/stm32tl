#include <string.h>
#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <stddef.h>
#include <io.h>

#include <drivers/hc_sr04.h>

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

typedef GPIO_OUTPUT_T<PC, 1> TRIGGER;
typedef GPIO_INPUT_T<PC, 0> ECHO;

typedef HC_SR04<TRIGGER, ECHO> DISTANCE_SENSOR;

typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON | BOARD_T::CONSOLE> BOARD;

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
	LED1::set_high();
	TRIGGER::init();
	ECHO::init();
	while (1) {
		LED2::toggle();
		CON::getc();
		CON::puts("Ping...\n");
		DISTANCE_SENSOR::measure();
	}
	return 0;
}
