#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <drivers/ringbuffer.h>
#include <usb/usb.h>
#include <drivers/cp2102.h>

#ifdef STM32F0xx
typedef SYSCLK_T<HSI48_RC> SYSCLK;
#else
typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
#endif
typedef SYSTICK_T<SYSCLK> SYSTICK;

#ifdef STM32F0xx
typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_DOWN> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN> SWCLK;
typedef GPIO_T<PB, 10, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SCL;
typedef GPIO_T<PB, 11, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SDA;
#else
typedef GPIO_INPUT_T<PA, 13> SWDIO;
typedef GPIO_INPUT_T<PA, 14> SWCLK;
typedef GPIO_T<PB, 10, OUTPUT_10MHZ, ALT_OPENDRAIN> SCL;
typedef GPIO_T<PB, 11, OUTPUT_10MHZ, ALT_OPENDRAIN> SDA;
#endif

typedef GPIO_OUTPUT_T<PB, 9> LED1;
typedef GPIO_OUTPUT_T<PB, 8> LED2;

typedef GPIO_PORT_T<PA, SWCLK, SWDIO> PORT_A;
typedef GPIO_PORT_T<PB, LED1, LED2> PORT_B;

typedef RINGBUFFER_T::BUFFER<0, uint8_t, 128> TX_BUFFER;
typedef RINGBUFFER_T::BUFFER<1, uint8_t, 128> RX_BUFFER;
typedef USB_T::CP2102<TX_BUFFER, RX_BUFFER> UART;

extern "C" {

void HardFault_Handler(void) {
	while (1);
}

#ifdef STM32F0xx
void USB_IRQHandler(void) {
#else
void USB_LP_CAN1_RX0_IRQHandler(void) {
#endif
	if (UART::handle_irq()) exit_idle();
}

}

int main(void)
{
	char buffer[128];

#ifdef STM32F0xx
	HSI48_RC::init();
#else
	HSE::init();
	PLL::init();
#endif
	SYSCLK::init();
	SYSTICK::init();
	PORT_A::init();
	PORT_B::init();
	UART::init();
	LED1::set_high();
	while (!UART::dtr() && !UART::rts()) {
		enter_idle();
	}
	LED1::set_low();
	SYSTICK::set_and_wait(100);
	UART::puts("Echo test\n");
	while(1) {
		LED2::toggle();
		while (!UART::gets(buffer, sizeof(buffer))) {
			LED1::set_high();
			enter_idle();
			LED1::set_low();
		}
		UART::putc('\n');
		UART::puts(buffer);
	}
	return 0;
}
