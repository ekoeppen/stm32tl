#pragma once

#include <stm32f1xx.h>
#include <clocks.h>

namespace I2C_T {

I2C_TypeDef I2C1_P_ __attribute__((section(".i2c1_peripheral")));
I2C_TypeDef * const I2C1_P = (I2C_TypeDef *) &I2C1_P_;

I2C_TypeDef I2C_P_ __attribute__((section(".i2c2_peripheral")));
I2C_TypeDef * const I2C_P = (I2C_TypeDef *) &I2C_P_;

static constexpr I2C_TypeDef *I2C_MODULE[2] = {I2C1_P, I2C_P};

template<typename SYSCLK,
	const int INSTANCE,
	const long FREQUENCY = 100000,
	const bool CLOCK_STRETCHING = false>
struct MASTER {
	static uint8_t slave_addr;
	static constexpr I2C_TypeDef *I2C = I2C_MODULE[INSTANCE - 1];

	static void init(void) {
		I2C->CR1 &= ~I2C_CR1_PE;
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
		switch (INSTANCE) {
		case 1:
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
			RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
			RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
			break;
		case 2:
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
			RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
			RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;
			break;
		}
		I2C->CR2 |= SYSCLK::pclk1 / 1000000;
		if (FREQUENCY <= 100000) {
			I2C->CCR = SYSCLK::pclk1 / (FREQUENCY * 2);
			I2C->TRISE = SYSCLK::pclk1 / 1000000 + 1;
		} else {
			I2C->CCR = SYSCLK::pclk1 / (FREQUENCY * 3) | I2C_CCR_FS;
			I2C->TRISE = (SYSCLK::pclk1 / 1000000) * 3 / 10 + 1;
		}
		I2C->CR1 |= I2C_CR1_PE | I2C_CR1_ACK;
	}

	static void enable(void) {
		switch (INSTANCE) {
		case 1: RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; break;
		case 2: RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; break;
		}
	}

	static void disable(void) {
		switch (INSTANCE) {
		case 1: RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN; break;
		case 2: RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN; break;
		}
	}

	static void set_slave_addr(uint8_t addr) {
		slave_addr = addr;
	}

	static void write(const uint8_t *data, uint32_t length) {
	}

	static void read(uint8_t *data, uint32_t length) {
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void generate_start(void) {
		I2C->CR1 |= I2C_CR1_START;
		while (!TIMEOUT::triggered() && !(I2C->SR1 & I2C_SR1_SB));
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void send_slave_addr(uint8_t slave_addr) {
		I2C->DR = slave_addr;
		while (!TIMEOUT::triggered() && !(I2C->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)));
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void wait_for_txe(void) {
		while ((CLOCK_STRETCHING && !TIMEOUT::triggered()) && !(I2C->SR1 & I2C_SR1_TXE));
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void wait_for_stop(void) {
		while (!TIMEOUT::triggered() && !(I2C->CR1 & I2C_CR1_STOP));
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void write_reg(const uint8_t reg, const uint8_t value) {
		generate_start<TIMEOUT>();
		send_slave_addr<TIMEOUT>(slave_addr << 1);
		I2C->SR2;
		wait_for_txe<TIMEOUT>();
		I2C->DR = reg;
		wait_for_txe<TIMEOUT>();
		I2C->DR = value;
		wait_for_txe<TIMEOUT>();
		I2C->CR1 |= I2C_CR1_STOP;
		wait_for_stop<TIMEOUT>();
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint8_t read_reg(const uint8_t reg) {
		uint8_t r = 0;

		generate_start<TIMEOUT>();
		send_slave_addr<TIMEOUT>(slave_addr << 1);
		r = I2C->SR2;
		wait_for_txe<TIMEOUT>();
		I2C->DR = reg;
		wait_for_txe<TIMEOUT>();
		generate_start<TIMEOUT>();
		send_slave_addr<TIMEOUT>((slave_addr << 1) | 1);
		I2C->CR1 &= ~I2C_CR1_ACK;
		r = I2C->SR2;
		I2C->CR1 |= I2C_CR1_STOP;
		wait_for_txe<TIMEOUT>();
		r = I2C->DR;
		wait_for_stop<TIMEOUT>();
		I2C->CR1 |= I2C_CR1_ACK;
		return r;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void read_reg(const uint8_t reg, uint8_t *data, uint32_t length) {
		uint8_t r;

		generate_start<TIMEOUT>();
		send_slave_addr<TIMEOUT>(slave_addr << 1);
		r = I2C->SR2;
		wait_for_txe<TIMEOUT>();
		I2C->DR = reg;
		wait_for_txe<TIMEOUT>();

		generate_start<TIMEOUT>();
		send_slave_addr<TIMEOUT>((slave_addr << 1) | 1);
		r = I2C->SR2;
		while (!TIMEOUT::triggered() && length > 0) {
			if (length != 3) {
				*data++ = I2C->DR;
				length--;
			}
			if (length == 3) {
				while (!TIMEOUT::triggered() && !(I2C->SR1 & I2C_SR1_BTF));
				I2C->CR1 &= ~I2C_CR1_ACK;
				*data++ = I2C->DR;
				I2C->CR1 |= I2C_CR1_STOP;
				*data++ = I2C->DR;
				while (!TIMEOUT::triggered() && !(I2C->SR1 & I2C_SR1_RXNE));
				*data++ = I2C->DR;
				length = 0;
			}
		}
		wait_for_stop<TIMEOUT>();
		I2C->CR1 |= I2C_CR1_ACK;
	}

	static bool addr_matched(void) {
		return I2C->SR1 & I2C_SR1_ADDR;
	}

	static void clear_addr_matched(void) {
		volatile uint32_t temp;
		temp = I2C->SR1;
		temp = I2C->SR2;
	}
};

template<typename SYSCLK, const int INSTANCE, const long FREQUENCY, const bool CLOCK_STRETCHING>
uint8_t MASTER<SYSCLK, INSTANCE, FREQUENCY, CLOCK_STRETCHING>::slave_addr;

typedef void(*ADDR_CALLBACK)(uint8_t addr);
typedef uint8_t(*TX_CALLBACK)(void);
typedef void(*RX_CALLBACK)(uint8_t data);

template<typename SYSCLK,
	const uint32_t INSTANCE,
	const uint8_t OWN_ADDRESS,
	const uint8_t OWN_ADDRESS2,
	ADDR_CALLBACK addr_callback,
	RX_CALLBACK rx_callback,
	TX_CALLBACK tx_callback>
struct SLAVE {
	static bool transmitting;
	static constexpr I2C_TypeDef *i2c = (INSTANCE == 1 ? I2C1 : I2C2);

	static void init(void) {
		i2c->CR1 &= ~I2C_CR1_PE;
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
		switch (INSTANCE) {
		case 1:
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
			RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
			RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
			break;
		case 2:
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
			RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
			RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;
			break;
		}
		i2c->CR2 |= SYSCLK::pclk1 / 1000000;
		i2c->OAR1 = (OWN_ADDRESS << 1) | 0x4000;
		if (OWN_ADDRESS2) {
			i2c->OAR2 = (OWN_ADDRESS2 << 1) | I2C_OAR2_ENDUAL;
		}
		transmitting = false;
		i2c->CR1 |= I2C_CR1_PE | I2C_CR1_ACK;
	}

	static void enable(void) {
		switch (INSTANCE) {
		case 1: RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; break;
		case 2: RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; break;
		}
	}

	static void disable(void) {
		switch (INSTANCE) {
		case 1: RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN; break;
		case 2: RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN; break;
		}
	}

	static bool handle_irq(void) {
		uint32_t sr1, sr2;

		sr1 = i2c->SR1;
		sr2 = i2c->SR1;
		if (sr1 & I2C_SR1_ADDR) {
			addr_callback(sr2 & I2C_SR2_DUALF ? OWN_ADDRESS2 : OWN_ADDRESS);
		}
		if (sr1 & I2C_SR1_TXE) {
			i2c->DR = tx_callback();
		}
		if (i2c->SR1 & I2C_SR1_RXNE) {
			rx_callback(i2c->DR);
		}
		if (i2c->SR1 & I2C_SR1_STOPF) {
			i2c->CR1 |= I2C_CR1_PE;
		}
		return true;
	}

};

template<typename SYSCLK, const uint32_t INSTANCE, const uint8_t OWN_ADDRESS, const uint8_t OWN_ADDRESS2,
	ADDR_CALLBACK addr_callback, RX_CALLBACK rx_callback, TX_CALLBACK tx_callback>
bool SLAVE<SYSCLK, INSTANCE, OWN_ADDRESS, OWN_ADDRESS2, addr_callback, rx_callback, tx_callback>::transmitting;

}
