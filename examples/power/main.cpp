#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <pwr.h>

#define BUTTON_PIN       7
#define LED_GREEN_PIN    9
#define LED_RED_PIN      8

typedef GPIO_OUTPUT_T<PB, LED_RED_PIN> LED_RED;
typedef GPIO_OUTPUT_T<PB, LED_GREEN_PIN> LED_GREEN;
typedef GPIO_INPUT_T<PB, BUTTON_PIN> BUTTON;
typedef GPIO_PORT_T<PB, LED_RED, LED_GREEN, BUTTON> PORT_B;

typedef PWR_T<DS_REGULATOR_LOW_POWER> POWER;

extern "C" {

void HardFault_Handler(void)
{
	PORT_B::init();
	while (1) {
		LED_RED::toggle();
		for (volatile int i = 0; i < 10000; i++);
	}
}

}

int main(void)
{
	POWER::init();
	PORT_B::init();
	LED_RED::set_low();
	LED_GREEN::set_low();
	while (1) {
		POWER::stop();
	}
}
