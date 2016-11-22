#include <clocks.h>
#include <gpio.h>
#include <uart.h>
#include <spi.h>
#include <pwr.h>
#include <rtc.h>
#include <blink.h>
#include <blink.h>

typedef SYSCLK_T<> SYSCLK;
typedef LSE_OSC_T<32768> LSECLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef PWR_T<> POWER;

#ifdef OLIMEXINO
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_RED;
typedef GPIO_T<PC, 13, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_GREEN;
typedef GPIO_PORT_T<PA, LED_RED> PORT_A;
typedef GPIO_PORT_T<PB> PORT_B;
typedef GPIO_PORT_T<PC, LED_GREEN> PORT_C;
#else
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_RED;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_GREEN;
typedef GPIO_PORT_T<PA> PORT_A;
typedef GPIO_PORT_T<PB, LED_RED, LED_GREEN> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
#endif

typedef RTC_T<LSECLK, 16> RTCLK;
typedef TIMEOUT_T<RTCLK> TIMEOUT;
typedef BLINK_T<RTCLK, LED_RED> BLINKER;

extern "C" {

void RTC_IRQHandler(void)
{
	RTCLK::handle_irq();
	BLINKER::handle_irq();
	if (TIMEOUT::count_down()) exit_idle();
}

}

int main(void)
{
	POWER::init();
	SYSCLK::init();
	LSECLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	RTCLK::init();
	while (1) {
		TIMEOUT::set_and_wait(1000);
		BLINKER::blink(3, 100);
		LED_GREEN::toggle();
	}
	return 0;
}
