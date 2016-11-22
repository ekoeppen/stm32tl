#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <uart.h>
#include <io.h>
#include <spi.h>

typedef SYSCLK_T<HSI_RC> SYSCLK;

typedef GPIO_OUTPUT_T<PA,  4> CSN;
typedef GPIO_OUTPUT_T<PA,  5> LED_RED;
#ifdef STM32F1xx
typedef GPIO_T<PA,  5, OUTPUT_10MHZ, ALT_PUSH_PULL> SCK;
typedef GPIO_T<PA,  6, OUTPUT_10MHZ, ALT_PUSH_PULL> MISO;
typedef GPIO_T<PA,  7, OUTPUT_10MHZ, ALT_PUSH_PULL> MOSI;
typedef GPIO_T<PA,  9, OUTPUT_10MHZ, ALT_PUSH_PULL> UART_TX;
typedef GPIO_T<PA, 10, INPUT> UART_RX;
#else
typedef GPIO_T<PA,  5, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> SCK;
typedef GPIO_T<PA,  6, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> MISO;
typedef GPIO_T<PA,  7, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> MOSI;
typedef GPIO_T<PA,  9, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF1> UART_TX;
typedef GPIO_T<PA, 10, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF1> UART_RX;
#endif
typedef GPIO_PORT_T<PA, SCK, MOSI, MISO, CSN, UART_RX, UART_TX> PORT_A;

typedef USART_T<SYSCLK, USART_1, 115200> SERIAL;

typedef SPI_T<SYSCLK, SPI_1, true, 0> SPI;

extern "C" {

void USART1_IRQHandler(void)
{
	if (SERIAL::handle_irq()) exit_idle();
}

}

int main(void)
{
	PORT_A::init();
	SERIAL::init();
	SPI::init();

	SERIAL::puts("SPI library example start\n");
	while (1) {
		char buffer[3 * 16 + 1];
		char data[16];
		int i, n, len;
		SERIAL::puts("Enter bytes to send: ");
		len = SERIAL::gets(buffer, sizeof(buffer));
		for (i = 0, n = 0; i < len; i += 3, n++) {
			data[n] = (FROM_HEX(buffer[i]) << 4) + FROM_HEX(buffer[i + 1]);
		}
		SERIAL::puts("Sending data: ");
		for (i = 0; i < n; i++) {
			printf<SERIAL>("%02x ", data[i]);
		}
		SERIAL::putc('\n');
		CSN::set_low();
		for (i = 0; i < n; i++) {
			data[i] = SPI::transfer(data[i]);
		}
		CSN::set_high();
		SERIAL::puts("Received data: ");
		for (int i = 0; i < n; i++) {
			printf<SERIAL>("%02x ", data[i]);
		}
		SERIAL::putc('\n');
	}
}
