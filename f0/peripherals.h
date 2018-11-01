#pragma once

typedef struct {
	__IO uint16_t USB_ADDR_TX;
	__IO uint16_t USB_COUNT_TX;
	__IO uint16_t USB_ADDR_RX;
	__IO uint16_t USB_COUNT_RX;
} USB_BTABLE_TypeDef;

extern GPIO_TypeDef GPIO_A; __asm(".equ GPIO_A, 0x48000000");
extern GPIO_TypeDef GPIO_B; __asm(".equ GPIO_B, 0x48000400");
extern GPIO_TypeDef GPIO_C; __asm(".equ GPIO_C, 0x48000800");
extern GPIO_TypeDef GPIO_D; __asm(".equ GPIO_D, 0x48000c00");
extern GPIO_TypeDef GPIO_F; __asm(".equ GPIO_F, 0x48001400");
extern USB_TypeDef USB_DEV; __asm(".equ USB_DEV, 0x48000000");
extern USB_BTABLE_TypeDef USB_BTABLE_ENTRY[1024 / sizeof(USB_BTABLE_TypeDef)]; __asm(".equ USB_BTABLE_ENTRY, 0x40006000");
extern uint8_t Unique_ID[12];
#ifdef STM32F0xx
asm(".equ Unique_ID, 0x1ffff7ac");
#else
asm(".equ Unique_ID, 0x1ffff7e8");
#endif

