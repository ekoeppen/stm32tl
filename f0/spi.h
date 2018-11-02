#pragma once

template<typename CLOCK,
	const bool MASTER = true,
	const int MODE = 3,
	const int SPEED = 1000000,
	const int DATA_LENGTH = 8,
	const bool LSB = false>
struct SPI_T {
	static constexpr uint32_t data_length = DATA_LENGTH;

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
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

		SPI1->CR1 = ((MODE & 0x1) ? SPI_CR1_CPHA : 0) |
			((MODE & 0x2) ? SPI_CR1_CPOL : 0) |
			(MASTER ? SPI_CR1_MSTR : 0) |
			(LSB ? SPI_CR1_LSBFIRST : 0) |
			baudrate(SPEED, CLOCK::pclk) << 3 |
			SPI_CR1_SSM | SPI_CR1_SSI;
		SPI1->CR2 = ((DATA_LENGTH - 1) << 8) | (DATA_LENGTH <= 8 ? SPI_CR2_FRXTH : 0);

		if (SPI1->SR & SPI_SR_MODF) {
			SPI1->CR1 = SPI1->CR1;
		}
		SPI1->CR1 |= SPI_CR1_SPE;
	}

	static void disable(void) {
		SPI1->CR1 &= ~SPI_CR1_SPE;
		RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
	}

	static int readable(void) {
		return SPI1->SR & SPI_SR_RXNE;
	}

	static int writeable(void) {
		return SPI1->SR & SPI_SR_TXE;
	}

	static void write(uint16_t data) {
		while (!writeable());
		if (DATA_LENGTH <= 8) {
			*(uint8_t *) &SPI1->DR = data;
		} else {
			SPI1->DR = (uint16_t) data;
		}
	}

	static uint16_t read(void) {
		while (!readable());
		return SPI1->DR;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint16_t transfer(uint16_t data) {
		uint16_t r;
		while (!TIMEOUT::triggered() && !(SPI1->SR & SPI_SR_TXE));
		if (DATA_LENGTH <= 8) {
			*(uint8_t *) &SPI1->DR = data;
		} else {
			*(uint16_t *) &SPI1->DR = (uint16_t) data;
		}
		while (!TIMEOUT::triggered() && !(SPI1->SR & SPI_SR_RXNE));
		r = SPI1->DR;
		return r;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void transfer(const uint8_t *tx_data, uint32_t count, uint8_t *rx_data = 0) {
		uint8_t rx;
		SPI1->CR1 &= ~SPI_CR1_RXONLY;
		while (count > 0) {
			while (!TIMEOUT::triggered() && !(SPI1->SR & SPI_SR_TXE));
			*(uint8_t *) &SPI1->DR = tx_data ? *tx_data++ : 0xff;
			while (!TIMEOUT::triggered() && !(SPI1->SR & SPI_SR_RXNE));
			rx = SPI1->DR;
			if (rx_data) {
				*rx_data++ = rx;
			}
			count--;
		}
		for (volatile int i = 0; i < 10; i++);
	}

	static void putc(char data) {
		transfer(data);
	}

	static void puts(char *data) {
		while (*data) {
			putc(*data++);
		};
	}

	static uint8_t receive(void) {
		return 0;
	}

	static bool handle_irq(void) {
		return true;
	}
};
