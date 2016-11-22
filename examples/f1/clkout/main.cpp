#include <clocks.h>
#include <tasks.h>
#include <gpio.h>

#define HSE_FREQ 8000000

typedef HSE_OSC_T<HSE_FREQ> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK, 1000> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

#if defined(OLIMEXINO)
typedef GPIO_T<PA, 8, OUTPUT_50MHZ, ALT_PUSH_PULL, LOW> MCO;
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> D3;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, PUSH_PULL> D7;
typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PA, 5,  OUTPUT_10MHZ, PUSH_PULL> LED1;
typedef GPIO_PORT_T<PA, D0, D1, D2, D3, D7, MCO, LED1> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, MCO, BUTTON> PORT_C;
#elif defined(MINIBOARD)
typedef GPIO_T<PA, 8, OUTPUT_50MHZ, ALT_PUSH_PULL, LOW> MCO;
typedef GPIO_T<PC, 13, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;

typedef GPIO_PORT_T<PA, MCO> PORT_A;
typedef GPIO_PORT_T<PB> PORT_B;
typedef GPIO_PORT_T<PC, LED1> PORT_C;
#else
typedef GPIO_T<PA, 8, OUTPUT_50MHZ, ALT_PUSH_PULL, LOW> MCO;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;

typedef GPIO_PORT_T<PA> PORT_A;
typedef GPIO_PORT_T<PB, LED1> PORT_B;
typedef GPIO_PORT_T<PC, MCO> PORT_C;
#endif

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}

}

int main(void)
{
	HSE::init();
	PLL::init();
	SYSCLK::init();
	SYSTICK::init();
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	while (1) {
		LED1::set_high();
		SYSTICK::set_and_wait(100);
		LED1::set_low();
		TIMEOUT::set_and_wait(900);
	}
	return 0;
}
