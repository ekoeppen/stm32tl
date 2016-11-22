#include <clocks.h>
#include <tasks.h>
#include <spi.h>
#include <gpio.h>
#include <exti.h>
#include <pwr.h>
#include <io.h>
#include <utils.h>
#include <usb/usb.h>
#include <drivers/cp2102.h>
#include <drivers/cc110x.h>
#include <drivers/ringbuffer.h>

#include <board.h>

typedef SYSCLK_T<> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;

#if (defined BOARD_ID_olimexino_stm32)
typedef GPIO_T<PA, 2, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> CC1101_IRQ;
typedef GPIO_OUTPUT_T<PA, 3> CC1101_CSN;
typedef GPIO_INPUT_T<PB, 13> MISO_IN;

#elif (defined BOARD_ID_lqfp48_usb_s)
#ifdef STM32F1xx
typedef GPIO_T<PA, 2, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> CC1101_IRQ;
#else
typedef GPIO_T<PA, 2, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> CC1101_IRQ;
#endif
typedef GPIO_OUTPUT_T<PA, 3> CC1101_CSN;
typedef GPIO_INPUT_T<PB, 13> MISO_IN;
#endif

typedef EXTI_T<CC1101_IRQ> EXT_INTERRUPT;

typedef SPI_T<SYSCLK, 0, true, 0, 4000000> SPI;
typedef CC110X_T<SPI, CC1101_CSN, MISO_IN, CC1101_IRQ, SYSTICK, 0> CC1101;
typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::SPI> BOARD;

extern "C" {

void SPI1_IRQHandler(void)
{
	if (SPI::handle_irq()) exit_idle();
}

#if (defined STM32F1xx || defined STM32F4xx)
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
	exit_idle();
}
#elif (defined STM32F0xx)
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

