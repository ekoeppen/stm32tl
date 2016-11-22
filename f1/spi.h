#pragma once

#include <stm32f1xx.h>
#include <tasks.h>
#include <clocks.h>
#include <utils.h>

enum {
	SPI_1 = 0,
	SPI_2,
#ifdef RCC_APB1ENR_SPI3EN
	SPI_3
#endif
};

static constexpr SPI_TypeDef *spi_addr[] = {
	SPI1,
	SPI2
#ifdef RCC_APB1ENR_SPI3EN
	SPI3
#endif
};

static constexpr uint32_t rcc_spi_en[] = {
	RCC_APB2ENR_SPI1EN, RCC_APB1ENR_SPI2EN,
#ifdef RCC_APB1ENR_SPI3EN
	RCC_APB1ENR_SPI3EN
#endif
};

static constexpr IRQn_Type nvic_spi_irqn[] = {
	SPI1_IRQn, SPI2_IRQn,
#ifdef RCC_APB1ENR_SPI3EN
	SPI1_IRQn
#endif
};

template<typename CLOCK,
	const int INSTANCE,
	const bool MASTER = true,
	const int MODE = 3,
	const int SPEED = 1000000,
	const int DATA_LENGTH = 8,
	const bool MSB = true,
	const bool POLLED = true>
struct SPI_T {
	static constexpr SPI_TypeDef *spi = (INSTANCE == SPI_1 ? SPI1 : (INSTANCE == SPI_2 ? SPI2 :
#ifdef RCC_APB1ENR_SPI3EN
				SPI3
#else
				0
#endif
				));
	volatile static int tx_count;
	volatile static int rx_count;
	static uint8_t *rx_buffer;
	static uint8_t *tx_buffer;


	static constexpr int baudrate(const int rate, const int base_clock) {
		return (rate >= base_clock / 2) ? 0 :
			(rate >= base_clock / 4) ? 1 :
			(rate >= base_clock / 8) ? 2 :
			(rate >= base_clock / 16) ? 3 :
			(rate >= base_clock / 32) ? 4 :
			(rate >= base_clock / 64) ? 5 :
			(rate >= base_clock / 128) ? 6 :
			(rate >= base_clock / 256) ? 7 : 7;
	}

	static void init(void) {
		(INSTANCE == SPI_1 ? RCC->APB2ENR : RCC->APB1ENR) |= rcc_spi_en[INSTANCE];

		spi->CR1 = ((MODE & 0x1) ? SPI_CR1_CPHA : 0) |
			((MODE & 0x2) ? SPI_CR1_CPOL : 0) |
			(MASTER ? SPI_CR1_MSTR : 0) |
			(MSB ? 0 : SPI_CR1_LSBFIRST) |
			baudrate(SPEED, CLOCK::frequency) << 3 |
			(DATA_LENGTH == 16 ? SPI_CR1_DFF : 0) |
			SPI_CR1_SSM | SPI_CR1_SSI;

		if (spi->SR & SPI_SR_MODF) {
			spi->CR1 = spi->CR1;
		}
		spi->CR1 |= SPI_CR1_SPE;
		if (!POLLED) {
			NVIC_EnableIRQ(nvic_spi_irqn[INSTANCE]);
			NVIC_ClearPendingIRQ(nvic_spi_irqn[INSTANCE]);
		}
	}

	static void disable(void) {
		spi->CR1 &= ~SPI_CR1_SPE;
		(INSTANCE == SPI_1 ? RCC->APB2ENR : RCC->APB1ENR) &= ~rcc_spi_en[INSTANCE];
	}

	static void enable_irq(void) {
		spi->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_TXEIE | SPI_CR2_ERRIE;
	}

	static void disable_irq(void) {
		spi->CR2 &= ~(SPI_CR2_RXNEIE | SPI_CR2_TXEIE | SPI_CR2_ERRIE);
	}

	static bool busy(void) {
		return spi->SR & SPI_SR_BSY;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint8_t transfer(uint8_t data) {
		uint8_t r;
		while (!TIMEOUT::triggered() && !(spi->SR & SPI_SR_TXE));
		spi->DR = data;
		while (!TIMEOUT::triggered() && !(spi->SR & SPI_SR_RXNE));
		r = spi->DR;
		return r;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void transfer(uint8_t *tx_data, uint32_t count, uint8_t *rx_data = 0) {
		if (POLLED) {
			uint8_t rx;
			spi->CR1 &= ~SPI_CR1_RXONLY;
			while (count > 0) {
				while (!TIMEOUT::triggered() && !(spi->SR & SPI_SR_TXE));
				spi->DR = tx_data ? *tx_data++ : 0xff;
				while (!TIMEOUT::triggered() && !(spi->SR & SPI_SR_RXNE));
				rx = spi->DR;
				if (rx_data) {
					*rx_data++ = rx;
				}
				count--;
			}
		} else {
			tx_buffer = tx_data;
			tx_count = count;
			rx_buffer = rx_data;
			rx_count = count;
			enable_irq();
			while (!TIMEOUT::triggered() && rx_count > 0) {
				enter_idle();
			}
			disable_irq();
		}
	}

	static bool handle_irq(void) {
		if (POLLED) {
			return true;
		} else {
			bool resume = false;
			uint16_t status = spi->SR;

			if (status & SPI_SR_RXNE) {
				uint16_t received_data = spi->DR;
				if (rx_count > 0) {
					if (rx_buffer) {
						*rx_buffer = received_data;
						rx_buffer++;
					}
					rx_count--;
				}
				if (rx_count == 0) {
					spi->CR2 &= ~(SPI_CR2_RXNEIE | ~SPI_CR2_ERRIE);
					resume = true;
				}
			}
			if (status & SPI_SR_TXE) {
				if (tx_count > 0) {
					if (tx_buffer) {
						spi->DR = *tx_buffer;
						tx_buffer++;
					} else {
						spi->DR = 0xff;
					}
					tx_count--;
				} else {
					spi->CR2 &= ~SPI_CR2_TXEIE;
				}
			}
			if (status & SPI_SR_OVR) {
				if (tx_count == 0) {
					spi->DR;
					spi->SR;
				}
				resume = true;
			}
			return resume;
		}
	}

};

template<typename CLOCK, const int INSTANCE, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB, const bool POLLED>
volatile int SPI_T<CLOCK, INSTANCE, MASTER, MODE, SPEED, DATA_LENGTH, MSB, POLLED>::tx_count;
template<typename CLOCK, const int INSTANCE, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB, const bool POLLED>
volatile int SPI_T<CLOCK, INSTANCE, MASTER, MODE, SPEED, DATA_LENGTH, MSB, POLLED>::rx_count;
template<typename CLOCK, const int INSTANCE, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB, const bool POLLED>
uint8_t *SPI_T<CLOCK, INSTANCE, MASTER, MODE, SPEED, DATA_LENGTH, MSB, POLLED>::rx_buffer;
template<typename CLOCK, const int INSTANCE, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB, const bool POLLED>
uint8_t *SPI_T<CLOCK, INSTANCE, MASTER, MODE, SPEED, DATA_LENGTH, MSB, POLLED>::tx_buffer;
