#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <i2c.h>
#include <usb/usb.h>
#include <drivers/cp2102.h>
#include <drivers/ringbuffer.h>

#ifdef STM32F0xx
typedef PLL_T<HSI_RC, 48000000> PLL;
#else
typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
#endif
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;

typedef GPIO_OUTPUT_T<PB, 9> LED1;
typedef GPIO_OUTPUT_T<PB, 8> LED2;
typedef GPIO_INPUT_T<PB, 7> BUTTON;

#ifdef STM32F0xx
typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_DOWN> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN> SWCLK;
typedef GPIO_T<PB, 10, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SCL;
typedef GPIO_T<PB, 11, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SDA;
#else
typedef GPIO_INPUT_T<PA, 13> SWDIO;
typedef GPIO_INPUT_T<PA, 14> SWCLK;
typedef GPIO_T<PB, 10, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SCL;
typedef GPIO_T<PB, 11, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SDA;
#endif

typedef GPIO_PORT_T<PA, SWCLK, SWDIO> PORT_A;
typedef GPIO_PORT_T<PB, LED1, LED2, SCL, SDA> PORT_B;

typedef RINGBUFFER::T<0, uint8_t, 128> TX_BUFFER;
typedef RINGBUFFER::T<1, uint8_t, 128> RX_BUFFER;
typedef USB_T::CP2102<TX_BUFFER, RX_BUFFER> UART;

uint8_t data[256];
uint8_t data_count;
volatile bool data_ready = false;
char buffer[4];

void trace(uint8_t d)
{
	data[data_count++] = d;
	if (data_count > sizeof(data)) data_count = 0;
	buffer[3] = 0;
	buffer[2] = ' ';
	buffer[1] = HEX_DIGIT(d & 0x0f);
	buffer[0] = HEX_DIGIT(d >> 4);
	data_ready = true;
}

void address_match(uint8_t addr) {
	LED1::toggle();
	trace(addr);
}

uint8_t tx_data(void) {
	trace(0x00);
	return 0x00;
}

void rx_data(uint8_t data) {
	trace(data);
}

#ifdef STM32F0xx
typedef I2C_T::SLAVE<SYSCLK, 2, 0x34, 0, 0x7, address_match, rx_data, tx_data> I2C_SLAVE;
#else
typedef I2C_T::SLAVE<SYSCLK, 2, 0x34, 0, address_match, rx_data, tx_data> I2C_SLAVE;
#endif

extern "C" {

#ifdef STM32F0xx
void USB_IRQHandler(void) {
#else
void USB_LP_CAN1_RX0_IRQHandler(void) {
#endif
	if (UART::handle_irq()) exit_idle();
}

void I2C2_IRQHandler(void) {
	if (I2C_SLAVE::handle_irq()) exit_idle();
}

}

int main(void)
{
	data_count = 0;
#ifdef STM32F0xx
	HSI48_RC::init();
#else
	HSE::init();
#endif
	PLL::init();
	SYSCLK::init();
	SYSTICK::init();
	PORT_A::init();
	PORT_B::init();
	UART::init();
	I2C_SLAVE::init();
	LED1::set_high();
	while(1) {
		enter_idle();
		if (data_ready) {
			data_ready = false;
			UART::puts(buffer);
		}
	}
	return 0;
}
