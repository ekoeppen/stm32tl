#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <exti.h>
#include <pwr.h>
#include <utils.h>

typedef PWR_T<> POWER;
typedef GPIO_T<PA, 5, MODE_OUTPUT> LED_RED;
typedef GPIO_T<PA, 6, MODE_OUTPUT> LED_GREEN;
typedef GPIO_T<PA, 2, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0, EDGE_FALLING> BUTTON;
typedef GPIO_PORT_T<PA, LED_RED, LED_GREEN, BUTTON> PORT_A;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;

volatile bool button_pressed;

extern "C" {

void HardFault_Handler(void)
{
	PORT_A::init();
	while (1) {
		LED_RED::toggle();
		delay(500000);
	}
}

void EXTI0_1_IRQHandler(void)
{
	button_pressed = true;
	EXTI->PR |= 0x0003;
}

void EXTI2_3_IRQHandler(void)
{
	button_pressed = true;
	EXTI->PR |= 0x000c;
}

void EXTI4_15_IRQHandler(void)
{
	button_pressed = true;
	EXTI->PR |= 0xff00;
}

}

void reset_exti(void)
{
	button_pressed = false;
	EXTI->PR = 0xffff;
	NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
	NVIC_ClearPendingIRQ(EXTI2_3_IRQn);
	NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
}

int main(void)
{
	POWER::init();
	PORT_A::init();
	EXT_INTERRUPT::init();

	while (1) {
		reset_exti();
		POWER::wfi();
		if (button_pressed) {
			LED_RED::toggle();
		}
	}
}
