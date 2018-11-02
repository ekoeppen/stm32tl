#pragma once

typedef struct {
	__IO uint16_t USB_ADDR_TX;
	__IO uint16_t USB_COUNT_TX;
	__IO uint16_t USB_ADDR_RX;
	__IO uint16_t USB_COUNT_RX;
} USB_BTABLE_TypeDef;

extern ADC_TypeDef P_ADC1;                  __asm(".equ P_ADC1, 0x40012400");
extern CAN_TypeDef P_CAN;                   __asm(".equ P_CAN, 0x40006400");
extern CEC_TypeDef P_CEC;                   __asm(".equ P_CEC, 0x40007800");
extern COMP_TypeDef P_COMP1;                __asm(".equ P_COMP1, 0x4001001c");
extern COMP_TypeDef P_COMP2;                __asm(".equ P_COMP2, 0x4001001c");
// extern CRC_TypeDef P_CRC;                   __asm(".equ P_CRC, 0x40000000");
// extern CRS_TypeDef P_CRS;                   __asm(".equ P_CRS, 0x40000000");
// extern DAC_TypeDef P_DAC1;                  __asm(".equ P_DAC1, 0x40000000");
// extern DAC_TypeDef P_DAC;                   __asm(".equ P_DAC, 0x40000000");
// extern DBGMCU_TypeDef P_DBGMCU;             __asm(".equ P_DBGMCU, 0x40000000");
// extern DMA_Channel_TypeDef P_DMA1_Channel1; __asm(".equ P_DMA1_Channel1, 0x40000000");
// extern DMA_Channel_TypeDef P_DMA1_Channel2; __asm(".equ P_DMA1_Channel2, 0x40000000");
// extern DMA_Channel_TypeDef P_DMA1_Channel3; __asm(".equ P_DMA1_Channel3, 0x40000000");
// extern DMA_Channel_TypeDef P_DMA1_Channel4; __asm(".equ P_DMA1_Channel4, 0x40000000");
// extern DMA_Channel_TypeDef P_DMA1_Channel5; __asm(".equ P_DMA1_Channel5, 0x40000000");
// extern DMA_Channel_TypeDef P_DMA1_Channel6; __asm(".equ P_DMA1_Channel6, 0x40000000");
// extern DMA_Channel_TypeDef P_DMA1_Channel7; __asm(".equ P_DMA1_Channel7, 0x40000000");
// extern DMA_TypeDef P_DMA1;                  __asm(".equ P_DMA1, 0x40000000");
// extern EXTI_TypeDef P_EXTI;                 __asm(".equ P_EXTI, 0x40000000");
// extern FLASH_TypeDef P_FLASH;               __asm(".equ P_FLASH, 0x40000000");
extern GPIO_TypeDef P_GPIOA;                __asm(".equ P_GPIOA, 0x48000000");
extern GPIO_TypeDef P_GPIOB;                __asm(".equ P_GPIOB, 0x48000400");
extern GPIO_TypeDef P_GPIOC;                __asm(".equ P_GPIOC, 0x48000800");
extern GPIO_TypeDef P_GPIOD;                __asm(".equ P_GPIOD, 0x48000c00");
extern GPIO_TypeDef P_GPIOE;                __asm(".equ P_GPIOE, 0x48001000");
extern GPIO_TypeDef P_GPIOF;                __asm(".equ P_GPIOF, 0x48001400");
extern I2C_TypeDef P_I2C1;                  __asm(".equ P_I2C1, 0x40005400");
extern I2C_TypeDef P_I2C2;                  __asm(".equ P_I2C2, 0x40005800");
// extern IWDG_TypeDef P_IWDG;                 __asm(".equ P_IWDG, 0x40000000");
// extern OB_TypeDef P_OB;                     __asm(".equ P_OB, 0x40000000");
// extern PWR_TypeDef P_PWR;                   __asm(".equ P_PWR, 0x40000000");
// extern RCC_TypeDef P_RCC;                   __asm(".equ P_RCC, 0x40000000");
// extern RTC_TypeDef P_RTC;                   __asm(".equ P_RTC, 0x40000000");
extern SPI_TypeDef P_SPI1;                  __asm(".equ P_SPI1, 0x40013000");
extern SPI_TypeDef P_SPI2;                  __asm(".equ P_SPI2, 0x40003800");
// extern SYSCFG_TypeDef P_SYSCFG;             __asm(".equ P_SYSCFG, 0x40000000");
// extern TIM_TypeDef P_TIM14;                 __asm(".equ P_TIM14, 0x40000000");
// extern TIM_TypeDef P_TIM15;                 __asm(".equ P_TIM15, 0x40000000");
// extern TIM_TypeDef P_TIM16;                 __asm(".equ P_TIM16, 0x40000000");
// extern TIM_TypeDef P_TIM17;                 __asm(".equ P_TIM17, 0x40000000");
// extern TIM_TypeDef P_TIM1;                  __asm(".equ P_TIM1, 0x40000000");
// extern TIM_TypeDef P_TIM2;                  __asm(".equ P_TIM2, 0x40000000");
// extern TIM_TypeDef P_TIM3;                  __asm(".equ P_TIM3, 0x40000000");
// extern TIM_TypeDef P_TIM6;                  __asm(".equ P_TIM6, 0x40000000");
// extern TIM_TypeDef P_TIM7;                  __asm(".equ P_TIM7, 0x40000000");
// extern TSC_TypeDef P_TSC;                   __asm(".equ P_TSC, 0x40000000");
extern USART_TypeDef P_USART1;              __asm(".equ P_USART1, 0x40013800");
extern USART_TypeDef P_USART2;              __asm(".equ P_USART2, 0x40004400");
extern USART_TypeDef P_USART3;              __asm(".equ P_USART3, 0x40004800");
extern USART_TypeDef P_USART4;              __asm(".equ P_USART4, 0x40004c00");
extern USB_TypeDef P_USB;                   __asm(".equ P_USB, 0x48000000");
// extern WWDG_TypeDef P_WWDG;                 __asm(".equ P_WWDG, 0x40000000");

extern uint8_t Unique_ID[12];
#ifdef STM32F0xx
asm(".equ Unique_ID, 0x1ffff7ac");
#else
asm(".equ Unique_ID, 0x1ffff7e8");
#endif
extern USB_BTABLE_TypeDef P_USB_BTABLE[1024 / sizeof(USB_BTABLE_TypeDef)]; __asm(".equ P_USB_BTABLE, 0x40006000");

