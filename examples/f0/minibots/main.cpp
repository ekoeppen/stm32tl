#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <exti.h>
#include <pwr.h>
#include <utils.h>
#include <adc.h>
#include <spi.h>

#include <boards/minibots.h>

typedef BOARD_T::BOARD<> BOARD;
typedef ADC_T::CONTINUOUS_DMA<I1, I2, I3, POT> INPUTS;

template<typename OUTPUT, const uint32_t PWM_RESOLUTION>
struct MOTORS_T {
	enum {
		CW = 0,
		CCW = 1
	};

	enum {
		M1 = 0,
		M2 = 1
	};

	static constexpr uint16_t M1_CW = BIT0 | BIT1;
	static constexpr uint16_t M1_CCW = BIT5 | BIT7;
	static constexpr uint16_t M2_CW = BIT2 | BIT3;
	static constexpr uint16_t M2_CCW = BIT4 | BIT6;

	static const uint16_t outputs[2][2];

	static uint32_t direction[2];
	static uint32_t speed[2];
	static uint32_t step;

	static void set_outputs(void) {
		uint16_t output = 0;

		if (step < speed[0]) {
			output = outputs[0][direction[0]];
		}
		if (step < speed[1]) {
			output |= outputs[1][direction[1]];
		}
		OUTPUTS::send(output);

		step++;
		if (step == PWM_RESOLUTION) step = 0;
	}
};

template<typename OUTPUT, const uint32_t PWM_RESOLUTION>
const uint16_t MOTORS_T<OUTPUT, PWM_RESOLUTION>::outputs[2][2] = { {M1_CW, M1_CCW}, {M2_CW, M2_CCW} };

template<typename OUTPUT, const uint32_t PWM_RESOLUTION>
uint32_t MOTORS_T<OUTPUT, PWM_RESOLUTION>::direction[2] = {CW, CW};

template<typename OUTPUT, const uint32_t PWM_RESOLUTION>
uint32_t MOTORS_T<OUTPUT, PWM_RESOLUTION>::speed[2] = {0, 0};

template<typename OUTPUT, const uint32_t PWM_RESOLUTION>
uint32_t MOTORS_T<OUTPUT, PWM_RESOLUTION>::step = 0;

typedef MOTORS_T<OUTPUTS, 10> MOTORS;

struct SWITCHES {
	static uint16_t debounce_state[3];
	static bool on[3];
	static bool edge_on[3];
	static bool edge_off[3];

	static void debounce(uint16_t *state) {
		for (int i = 0; i < 3; i++) {
			debounce_state[i] = (debounce_state[i] << 1) | (state[i] < 0x700) | 0xe000;
			edge_on[i] = (debounce_state[i] == 0xf000);
			if (edge_on[i]) {
				on[i] = true;
			}
			edge_off[i] = (debounce_state[i] == 0xefff);
			if (edge_off[i]) {
				on[i] = false;
			}
		}
	}
};

bool SWITCHES::on[3] = {false, false, false};
bool SWITCHES::edge_on[3] = {false, false, false};
bool SWITCHES::edge_off[3] = {false, false, false};
uint16_t SWITCHES::debounce_state[3] = {0, 0, 0};

extern "C" {

void HardFault_Handler(void)
{
	while (1) {
	}
}

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}

void DMA1_Channel1_IRQHandler(void) {
	if ((DMA1->ISR & DMA_ISR_TEIF1) != 0) {
		DMA1->IFCR |= DMA_IFCR_CTEIF1;
	}
}

#ifdef STM32F030
void ADC1_IRQHandler(void) {
#else
void ADC1_COMP_IRQHandler(void) {
#endif
	if ((ADC1->ISR & ADC_ISR_OVR) != 0) {
		ADC1->ISR |= ADC_ISR_OVR; /* Clears the pending bit */
		ADC1->CR |= ADC_CR_ADSTP; /* Stop the sequence conversion */
	}
}
}

uint16_t inputs[4];
volatile uint8_t dip;

void on_off_test(void) {
	while (1) {
		SWITCHES::debounce(inputs);
		if (SWITCHES::on[2]) {
			LED::set_high();
		} else {
			LED::set_low();
		}
		TIMEOUT::set_and_wait(10);
	}
}

void led_blink_test(void) {
	while (1) {
		LED::toggle();
		TIMEOUT::set_and_wait(inputs[3] + 10);
	}
}

void motor_test(void) {
	uint16_t output;
	while (1) {
		SWITCHES::debounce(inputs);
		output = 0;
		if (SWITCHES::on[0]) {
			output |= BIT0 | BIT1;
		}
		if (SWITCHES::on[2]) {
			output |= BIT2 | BIT3;
		}
		OUTPUTS::send(output);
		LED::toggle();
		TIMEOUT::set_and_wait(10);
	}
}

void pwm_test(void) {
	while (1) {
		TIMEOUT::set_us(20);
		LED::toggle();
		SWITCHES::debounce(inputs);
		if (SWITCHES::on[0]) {
			MOTORS::speed[0] = inputs[3] * 10 / 4095;
		} else {
			MOTORS::speed[0] = 0;
		}
		MOTORS::set_outputs();
		while (!TIMEOUT::triggered()) {
			enter_idle();
		}
		TIMEOUT::disable();
	}
}

void obstacle_avoider(void) {
	enum {
		DRIVING,
		OBSTACLE_PAUSE,
		BACKUP,
		BACKUP_PAUSE,
		TURN,
		TURN_PAUSE
	} state;
	enum {
		OBSTACLE_NONE,
		OBSTACLE_LEFT,
		OBSTACLE_RIGHT
	} obstacle;
	uint32_t tick_us;

	MOTORS::speed[0] = MOTORS::speed[1] = 10;
	MOTORS::direction[0] = MOTORS::direction[1] = MOTORS::CW;
	state = DRIVING;
	tick_us = 0;
	obstacle = OBSTACLE_NONE;
	while (1) {
		TIMEOUT::set_us(100);
		MOTORS::set_outputs();
		SWITCHES::debounce(inputs);
		switch (state) {
			case DRIVING:
				if (SWITCHES::on[0] || SWITCHES::on[2]) {
					state = OBSTACLE_PAUSE;
					MOTORS::speed[0] = MOTORS::speed[1] = 0;
					tick_us = 0;
					if (SWITCHES::on[0]) {
						obstacle = OBSTACLE_LEFT;
					} else {
						obstacle = OBSTACLE_RIGHT;
					}
				}
				break;
			case OBSTACLE_PAUSE:
				if (tick_us > 500000) {
					state = BACKUP;
					MOTORS::speed[0] = MOTORS::speed[1] = 10;
					MOTORS::direction[0] = MOTORS::direction[1] = MOTORS::CCW;
					tick_us = 0;
				}
				break;
			case BACKUP:
				if (tick_us > 1000000) {
					state = BACKUP_PAUSE;
					MOTORS::speed[0] = MOTORS::speed[1] = 0;
					tick_us = 0;
				}
				break;
			case BACKUP_PAUSE:
				if (tick_us > 500000) {
					state = BACKUP;
					MOTORS::speed[0] = MOTORS::speed[1] = 10;
					if (obstacle == OBSTACLE_LEFT) {
						MOTORS::direction[0] = MOTORS::CW;
						MOTORS::direction[1] = MOTORS::CCW;
					} else {
						MOTORS::direction[0] = MOTORS::CCW;
						MOTORS::direction[1] = MOTORS::CW;
					}
					tick_us = 0;
					state = TURN;
				}
				break;
			case TURN:
				if (tick_us > 250000) {
					state = TURN_PAUSE;
					MOTORS::speed[0] = MOTORS::speed[1] = 0;
					tick_us = 0;
				}
				break;
			case TURN_PAUSE:
				if (tick_us > 1000000) {
					state = DRIVING;
					MOTORS::speed[0] = MOTORS::speed[1] = 10;
					MOTORS::direction[0] = MOTORS::direction[1] = MOTORS::CW;
					tick_us = 0;
				}
				break;
		}
		while (!TIMEOUT::triggered()) {
			enter_idle();
		}
		TIMEOUT::disable();
		tick_us += 200;
	}
}

int main(void)
{
	BOARD::init();
	INPUTS::init();
	DIP_CSN::set_low();

	LED::set_low();
	INPUTS::sample<TIMEOUT_IMMEDIATELY>(inputs, false);

	DIP_SPI::init();
	dip = DIP::read() & 0x1f;

	OUTPUTS_CSN::set_high();
	OUTPUTS_SPI::init();
	OUTPUTS::init();

	switch (dip) {
		case 0: led_blink_test(); break;
		case 1: on_off_test(); break;
		case 2: motor_test(); break;
		case 3: pwm_test(); break;
		case 4: obstacle_avoider(); break;
	}

	while (1) {
		enter_idle();
	}
}
