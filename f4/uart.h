#pragma once

#include <stm32f4xx.h>
#include <tasks.h>
#include <utils.h>

static constexpr USART_TypeDef *usart_addr[] = {USART1, USART2, USART3,
};

static constexpr uint32_t rcc_usart_en[] = {
	RCC_APB2ENR_USART1EN, RCC_APB1ENR_USART2EN, RCC_APB1ENR_USART3EN,
};

static constexpr IRQn_Type nvic_usart_irqn[] = {
	USART1_IRQn, USART2_IRQn, USART3_IRQn,
};

struct USART_STATUS {
	bool framing_error: 1;
	bool rx_available: 1;
};

enum {
	USART_1 = 1,
	USART_2,
	USART_3
};


template<typename CLOCK,
	const int INSTANCE,
	const int BAUD = 115200,
	const int WORD_LEN = 8,
	const int PARITY = 0,
	const int STOP_BITS = 1>
struct USART_T {
	static constexpr USART_TypeDef *uart =
		(INSTANCE == USART_3 ? USART3 : (INSTANCE == 2 ? USART2 : USART1));

	static USART_STATUS status;
	static volatile int tx_count;
	static volatile uint8_t rx_count;
	static volatile uint8_t rx_max;
	static uint8_t* rx_buffer;
	static uint8_t *tx_buffer;

	static void init(void) {
		(INSTANCE == USART_1 ? RCC->APB2ENR : RCC->APB1ENR) |= rcc_usart_en[INSTANCE - 1];
		uart->BRR = CLOCK::frequency / BAUD;
		uart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
		NVIC_EnableIRQ(nvic_usart_irqn[INSTANCE - 1]);
		NVIC_ClearPendingIRQ(nvic_usart_irqn[INSTANCE - 1]);
	}

	static bool enabled(void) { return true; }

	static void enable(void) {
		(INSTANCE == USART_1 ? RCC->APB2ENR : RCC->APB1ENR) |= rcc_usart_en[INSTANCE - 1];
	}

	static void disable(void) {
		(INSTANCE == USART_1 ? RCC->APB2ENR : RCC->APB1ENR) &= ~rcc_usart_en[INSTANCE - 1];
	}

	static void enable_tx_irq(void) {
		uart->CR1 |= USART_CR1_TXEIE;
	}

	static void disable_tx_irq(void) {
		uart->CR1 &= ~USART_CR1_TXEIE;
	}

	static bool tx_complete(void) {
		return uart->SR & USART_SR_TC;
	}

	static void enable_rx_irq(void) {
		uart->CR1 |= USART_CR1_RXNEIE;
	}

	static void disable_rx_irq(void) {
		uart->CR1 &= ~USART_CR1_RXNEIE;
	}

	static void clear_rx_irq(void) {
		uart->SR &= ~USART_SR_RXNE;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void transfer(uint8_t *tx_data, int count) {
		tx_buffer = tx_data;
		tx_count = count;
		enable_tx_irq();
		while (!TIMEOUT::triggered() && tx_count > 0) {
			enter_idle();
		}
		while (!TIMEOUT::triggered() && !tx_complete());
	}

	static bool handle_irq(void) {
		bool resume = false;
		uint32_t status = uart->SR;

		if ((status & USART_SR_TXE)) {
			if (tx_count > 0) {
				uart->DR = *tx_buffer;
				tx_buffer++;
				tx_count--;
			} else {
				disable_tx_irq();
				resume = true;
			}
		}
		if (status & USART_SR_RXNE) {
			if (rx_buffer && rx_count < rx_max) {
				*rx_buffer = uart->DR;
				rx_buffer++;
				rx_count++;
			}
			clear_rx_irq();
			resume = true;
		}
		return resume;
	}

	static bool handle_rx_irq(void) {
		bool resume = false;

		return resume;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void putc(char data) {
		uart->DR = data;
		while (!TIMEOUT::triggered() && !tx_complete());
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void puts(const char *data) {
		unsigned n;

		if (1) {
			for (n = 0; data[n] != '\0'; n++);
			transfer<TIMEOUT>((uint8_t *) data, n);
		} else {
			for (n = 0; data[n] != '\0'; n++) {
				putc<TIMEOUT>((uint8_t) data[n]);
			}
		}
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static char getc() {
		char c;
		status.rx_available = false;
		rx_count = 0;
		rx_max = 1;
		rx_buffer = (uint8_t *) &c;
		while (!TIMEOUT::triggered() && rx_count == 0) {
			enter_idle();
		}
		if (!TIMEOUT::triggered()) status.rx_available = true;
		return c;
	}


	template<typename TIMEOUT = TIMEOUT_NEVER>
	static int gets(char *s, int max) {
		status.rx_available = false;
		rx_count = 0;
		rx_max = max;
		rx_buffer = (uint8_t *) s;
		while (!TIMEOUT::triggered() && (rx_count == 0 || rx_buffer[rx_count - 1] != '\n')) {
			enter_idle();
		}
		return rx_count;
	}
};

template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
volatile int USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::tx_count;
template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
volatile uint8_t USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::rx_count;
template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
volatile uint8_t USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::rx_max;
template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
uint8_t *USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::rx_buffer;
template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
uint8_t *USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::tx_buffer;
template<typename CLOCK, const int INSTANCE, const int BAUD, const int WORD_LEN, const int PARITY, const int STOP_BITS>
USART_STATUS USART_T<CLOCK, INSTANCE, BAUD, WORD_LEN, PARITY, STOP_BITS>::status;
