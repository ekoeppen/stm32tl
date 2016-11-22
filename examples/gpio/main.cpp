#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <utils.h>

#include <board.h>

typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON> BOARD;

int main(void)
{
	BOARD::init();
	LED1::set_low();
	LED2::set_high();
	while (1) {
		LED2::set_value(!BUTTON::get_value());
		LED1::toggle();
		delay(1000000);
	}
}
