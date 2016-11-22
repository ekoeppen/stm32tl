extern void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void);
extern unsigned int __etext, __data_start__, __data_end__, __bss_start__, __bss_end__, __stack;
extern int main(void);

void Reset_Handler(void) __attribute__((naked));
void NMI_Handler(void) __attribute((naked));
void HardFault_Handler(void) __attribute__((weak, naked));
void Default_Handler(void) __attribute((naked));

void NMI_Handler(void)
{
	while (1);
}

void Default_Handler(void)
{
	__asm volatile("mrs r0, ipsr");
}

void HardFault_Handler(void)
{
	__asm volatile("mrs r0, ipsr");
}

void Reset_Handler(void)
{
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
	main();
	while (1);
}

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USBWakeUp_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	USB_LP_CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	USBWakeUp_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void (*)()) 0xF108F85F};
