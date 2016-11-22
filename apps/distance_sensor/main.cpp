#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <stddef.h>
#include <io.h>

#include <drivers/hc_sr04.h>

typedef PLL_T<HSI_RC, 32000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

typedef GPIO_OUTPUT_T<PF, 0> TRIGGER1;
typedef GPIO_OUTPUT_T<PF, 1> TRIGGER2;
typedef GPIO_OUTPUT_T<PA, 0> TRIGGER3;
typedef GPIO_OUTPUT_T<PA, 1> TRIGGER4;
typedef GPIO_INPUT_T<PA, 4> ECHO1;
typedef GPIO_INPUT_T<PA, 5> ECHO2;
typedef GPIO_INPUT_T<PA, 6> ECHO3;
typedef GPIO_INPUT_T<PA, 7> ECHO4;
typedef GPIO_T<PA, 13, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_HIGH, PULL_DOWN> SWDIO;
typedef GPIO_T<PA, 14, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN> SWCLK;
typedef GPIO_T<PB, 10, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SCL;
typedef GPIO_T<PB, 11, MODE_ALTFUNC, TYPE_OPENDRAIN, SPEED_HIGH, PULL_UP, AF1> SDA;
typedef GPIO_OUTPUT_T<PB, 1> IRQ;

typedef GPIO_PORT_T<PA, TRIGGER3, TRIGGER4, ECHO1, ECHO2, ECHO3, ECHO4, SWDIO, SWCLK> PORT_A;
typedef GPIO_PORT_T<PB, IRQ, SDA, SCL> PORT_B;
typedef GPIO_PORT_T<PF, TRIGGER1, TRIGGER2> PORT_F;

template<typename CLOCK>
struct SIMPLE_COUNTER_T {
	static void init(void) {
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		TIM3->ARR = 0xffff;
	}

	static void set_frequency(uint32_t f) {
		TIM3->PSC = CLOCK::frequency / f;
	}

	static void start(void) {
		TIM3->CNT = 0;
		TIM3->SR = 0;
		TIM3->CR1 = TIM_CR1_CEN;
	}

	static uint32_t stop(void) {
		TIM3->CR1 = 0;
		return TIM3->CNT;
	}

	static uint32_t value(void) {
		return TIM3->CNT;
	}

	static bool overflow(void) {
		return TIM3->SR & TIM_SR_UIF;
	}

};

typedef SIMPLE_COUNTER_T<SYSCLK> COUNTER;

typedef HC_SR04<COUNTER, TRIGGER1, ECHO1> SENSOR1;
typedef HC_SR04<COUNTER, TRIGGER2, ECHO2> SENSOR2;
typedef HC_SR04<COUNTER, TRIGGER3, ECHO3> SENSOR3;
typedef HC_SR04<COUNTER, TRIGGER4, ECHO4> SENSOR4;

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}

}

int main(void)
{
	PLL::init();
	SYSCLK::init();
	SYSTICK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_F::init();
	COUNTER::init();
	SENSOR1::init();
	SENSOR2::init();
	SENSOR3::init();
	SENSOR4::init();
	while (1) {
		if (SENSOR1::measure() < 30) {
			IRQ::set_high();
			TIMEOUT::set_and_wait(100);
			IRQ::set_low();
		}
		TIMEOUT::set_and_wait(1000);
	}
	return 0;
}
