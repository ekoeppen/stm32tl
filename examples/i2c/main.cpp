#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <i2c.h>
#include <io.h>

#include <board.h>

typedef SYSCLK_T<> SYSCLK;
typedef BOARD_T::BOARD<BOARD_T::CONSOLE | BOARD_T::LED1 | BOARD_T::LED2> BOARD;

typedef I2C_T::MASTER<SYSCLK, 2, 0x5e> I2C_MASTER;

#if (defined BOARD_ID_stm32_e407)
typedef USART_T<SYSCLK, USART_1> CON;

extern "C" {

void USART1_IRQHandler(void)
{
	if (CON::handle_irq()) exit_idle();
}

}

#endif

int main(void)
{
	BOARD::init();
	I2C_MASTER::init();
	LED1::set_high();
	while(1) {
		uint8_t value;
		CON::puts("Press any key...\n");
		CON::getc();
		LED2::toggle();
		I2C_MASTER::write_reg(0x12, 0x34);
		value = I2C_MASTER::read_reg(0x4d);
		printf<CON>("Register %x: %x\n", 0x4d, value);
	}
	return 0;
}
