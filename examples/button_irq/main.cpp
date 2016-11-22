#include <clocks.h>
#include <gpio.h>
#include <tasks.h>
#include <exti.h>

#include <board.h>

typedef SYSCLK_T<> SYSCLK;
typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON> BOARD;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}


#if (defined STM32F1xx) || (defined STM32F4xx)
void EXTI0_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0>();
	exit_idle();
}

void EXTI1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<1>();
	exit_idle();
}

void EXTI2_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2>();
	exit_idle();
}

void EXTI3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<3>();
	exit_idle();
}

void EXTI4_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4>();
	exit_idle();
}

void EXTI9_5_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<5, 9>();
	exit_idle();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
}
#else
void EXTI0_1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0, 1>();
	exit_idle();
}

void EXTI2_3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2, 3>();
	exit_idle();
}

void EXTI4_15_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4, 15>();
	exit_idle();
}

#endif

}

int main(void)
{
	BOARD::init();
	EXT_INTERRUPT::init();
	SYSTICK::init();
	while (1) {
		BUTTON::clear_irq();
		TIMEOUT::set(1000);
		if (BUTTON::template wait_for_irq<TIMEOUT>()) {
			LED1::set_high();
		} else {
			LED2::set_high();
		}
		SYSTICK::set_and_wait(100);
		LED1::set_low();
		LED2::set_low();
	}
	return 0;
}
