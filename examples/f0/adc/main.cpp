#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <drivers/ringbuffer.h>
#include <usb/usb.h>
#include <usb/cp2102.h>
#include <adc.h>

typedef PLL_T<HSI_RC, 48000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

typedef GPIO_T<PA, 0, MODE_ANALOG> P7;
typedef GPIO_T<PA, 1, MODE_ANALOG> P6;
typedef GPIO_T<PA, 2, MODE_ANALOG> P5;
typedef GPIO_T<PA, 3, MODE_ANALOG> P4;
typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_DOWN> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN> SWCLK;

typedef GPIO_T<PB, 9, MODE_OUTPUT> LED1;
typedef GPIO_T<PB, 8, MODE_OUTPUT> LED2;
typedef GPIO_T<PB, 7, MODE_INPUT> BUTTON;

typedef GPIO_PORT_T<PA, P4, P5, P6, P7, SWCLK, SWDIO> PORT_A;
typedef GPIO_PORT_T<PB, LED1, LED2> PORT_B;

typedef RINGBUFFER::T<0, uint8_t, 128> TX_BUFFER;
typedef RINGBUFFER::T<1, uint8_t, 128> RX_BUFFER;
typedef USB_T::CP2102<TX_BUFFER, RX_BUFFER> UART;

typedef ADC_T::CONTINUOUS<P4, P5, P6, P7> INPUTS;

extern "C" {

void HardFault_Handler(void) {
	while (1);
}

void USB_IRQHandler(void) {
	if (UART::handle_irq()) exit_idle();
}

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}

void ADC1_COMP_IRQHandler(void) {
	if (INPUTS::handle_irq()) exit_idle();
}

}

uint16_t samples[4];

int main(void)
{
	HSI48_RC::init();
	PLL::init();
	SYSCLK::init();
	SYSTICK::init();
	PORT_A::init();
	PORT_B::init();
	UART::init();
	INPUTS::init();
	LED1::set_high();
	UART::getc();
	while(1) {
		LED1::toggle();
		INPUTS::sample(samples, true);
		printf<UART>("Samples: %d %d %d %d\n", samples[0], samples[1], samples[2], samples[3]);
		TIMEOUT::set_and_wait(1000);
	}
	return 0;
}
