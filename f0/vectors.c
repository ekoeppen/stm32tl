#define MINIMUM_RAM 0x20001000

void Default_Handler(void)
{
	while (1) ;
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_15_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TS_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_COMP_IRQHandler (void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM14_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM15_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM16_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM17_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CEC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void init_segments(void)
{
	extern unsigned int __etext, __data_start__, __data_end__, __bss_start__, __bss_end__;
	unsigned int *src, *dst;

	src = &__etext;
	dst = &__data_start__;
	while (dst < &__data_end__) {
		*dst = *src;
		src++;
		dst++;
	}

	dst = &__bss_start__;
	while (dst < &__bss_end__) {
		*dst = 0;
		dst++;
	}
}

extern int main(void);

void Reset_Handler(void)
{
	init_segments();
	main();
	while (1);
}

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void *) MINIMUM_RAM,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	0, 0, 0, 0, 0, 0, 0,
	SVC_Handler,
	0, 0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_1_IRQHandler,
	EXTI2_3_IRQHandler,
	EXTI4_15_IRQHandler,
	TS_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_3_IRQHandler,
	DMA1_Channel4_5_IRQHandler,
	ADC1_COMP_IRQHandler,
	TIM1_BRK_UP_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM6_DAC_IRQHandler,
	0,
	TIM14_IRQHandler,
	TIM15_IRQHandler,
	TIM16_IRQHandler,
	TIM17_IRQHandler,
	I2C1_IRQHandler,
	I2C2_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	0,
	CEC_IRQHandler,
	USB_IRQHandler,
};
