#ifndef __UART_H
#define __UART_H

#include <stm32f0xx.h>
#include <stdarg.h>
#include "utils.h"

enum {
	USART_1 = 0,
	USART_2
};

template<typename CLOCK,
	const int INSTANCE,
	const int BAUD = 115200,
	const int WORD_LEN = 8,
	const int PARITY = 0,
	const int STOP_BITS = 1>
struct USART_T {
#ifdef USART2
	static constexpr USART_TypeDef *uart = INSTANCE == USART_1 ? &P_USART1 : &P_USART2;
#else
	static constexpr USART_TypeDef *uart = &P_USART1;
#endif

	volatile static bool irq_triggered;
	volatile static uint8_t last_rx_data;
	static constexpr bool enabled(void) {
		return true;
	}

	static void init(void) {
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		uart->BRR = CLOCK::frequency / BAUD / 4;
		uart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_ClearPendingIRQ(USART1_IRQn);
		USART1->ICR = 0xffffffff;
		irq_triggered = false;
		last_rx_data = 0;
	}

	static void disable(void) {
		USART1->CR1 &= ~USART_CR1_UE;
		RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
	}

	static bool tx_complete(void) {
		return (uart->ISR & USART_ISR_TXE);
	}

	static void enable_irq(void) {
	}

	static void disable_irq(void) {
	}

	static void send(uint8_t data) {
		uart->TDR = data;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void putc(char data) {
		uart->TDR = data;
		while (!tx_complete());
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void puts(const char *data) {
		while (*data) {
			putc(*data++);
		};
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static char getc(void) {
		irq_triggered = false;
		while (!TIMEOUT::triggered() && !irq_triggered) {
			enter_idle();
		}
		return last_rx_data;
	}

	template<bool ECHO = true>
	static uint32_t gets(char *buffer, uint32_t count) {
		uint32_t n = 0;

		while (count > 1) {
			irq_triggered = false;
			__WFI();
			if (irq_triggered) {
				if (ECHO) {
					putc(last_rx_data);
				}
				if (last_rx_data != 10 && last_rx_data != 4) {
					*buffer++ = last_rx_data;
					count--;
					n++;
				} else {
					break;
				}
			}
		}
		*buffer = 0;
		return n;
	}

	static bool handle_irq(void) {
		if (uart->ISR & USART_ISR_RXNE) {
			last_rx_data = USART1->RDR;
			irq_triggered = true;
		}
		uart->ICR = 0xffffffff;
		NVIC_ClearPendingIRQ(USART1_IRQn);
		return irq_triggered;
	}

};

template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
volatile bool USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::irq_triggered;

template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
volatile uint8_t USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::last_rx_data;

#endif
