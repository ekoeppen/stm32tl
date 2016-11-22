#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <uart.h>
#include <exti.h>
#include <pwr.h>
#include <io.h>

typedef SYSCLK_T<HSI_RC> SYSCLK;

typedef GPIO_OUTPUT_T<PA,  6> LED_RED;
#ifdef STM32F0xx
typedef GPIO_T<PA,  9, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF1> UART_TX;
typedef GPIO_T<PA, 10, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF1> UART_RX;
typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_UP, AF0> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN, AF0> SWCLK;
#else
typedef GPIO_INPUT_T<PA, 13> SWDIO;
typedef GPIO_INPUT_T<PA, 14> SWCLK;
typedef GPIO_T<PA,  9, OUTPUT_10MHZ, ALT_PUSH_PULL> UART_TX;
typedef GPIO_T<PA, 10, INPUT> UART_RX;
#endif
typedef GPIO_PORT_T<PA, LED_RED, UART_RX, UART_TX, SWDIO, SWCLK> PORT_A;

typedef USART_T<SYSCLK, 0, 115200, 8, 0, 1> SERIAL;

extern "C" {

void HardFault_Handler(void)
{
	PORT_A::init();
	while (1) {
		LED_RED::toggle();
		delay(500000);
	}
}

void USART1_IRQHandler(void)
{
	SERIAL::handle_irq();
}

}

int main(void)
{
	PORT_A::init();
	SERIAL::init();

	printf<SERIAL>("UART library example start\n");
	while (1) {
		char c, buffer[33];
		int n;
		printf<SERIAL>("Press any key...\n");
		c = SERIAL::getc();
		LED_RED::toggle();
		printf<SERIAL>("Key pressed: %c (code %d, hex %02x)\n", c, c, c);
		printf<SERIAL>("Enter a string: ");
		n = SERIAL::gets(buffer, sizeof(buffer));
		printf<SERIAL>("[%s] (%d)\n", buffer, n);
	}
}
