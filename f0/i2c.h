#pragma once

#include <utils.h>

namespace I2C_T {

template<typename SYSCLK,
	const uint32_t INSTANCE,
	const uint8_t SLAVE_ADDR,
	const uint32_t FREQUENCY = 100000>
struct MASTER {
	static uint8_t slave_addr;
	static constexpr auto i2c = mem_ptr<I2C_TypeDef>(INSTANCE == 1 ? I2C1_BASE : I2C2_BASE);

	static void init(void) {
		slave_addr = SLAVE_ADDR;
		i2c->CR1 &= ~I2C_CR1_PE;
		RCC->APB1ENR |= (INSTANCE == 1 ? RCC_APB1ENR_I2C1EN : RCC_APB1ENR_I2C2EN);
		i2c->TIMINGR = 0xb0420f13;
		i2c->CR1 = I2C_CR1_PE;
	}

	static void enable(void) {
	}

	static void disable(void) {
	}

	static void set_slave_addr(const uint8_t addr) {
		slave_addr = addr;
	}

	static void stop(void) {
	}

	static bool slave_present(const uint8_t slave_address) {
		return false;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void write(const uint8_t *data, int length, bool restart = false) {
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void read(uint8_t *data, uint16_t length) {
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void write_reg(const uint8_t reg, const uint8_t value) {
		i2c->CR2 = (2 << 16) | (slave_addr << 1);
		i2c->CR2 |= I2C_CR2_START;
		while (!TIMEOUT::triggered() && !(i2c->ISR & I2C_ISR_TXIS));
		i2c->TXDR = reg;
		while (!TIMEOUT::triggered() && !(i2c->ISR & I2C_ISR_TXIS));
		i2c->TXDR = value;
		while (!TIMEOUT::triggered() && !(i2c->ISR & I2C_ISR_TC));
		i2c->CR2 = I2C_CR2_STOP;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint8_t read_reg(const uint8_t reg) {
		uint8_t r;

		i2c->CR2 = (1 << 16) | (slave_addr << 1);
		i2c->CR2 |= I2C_CR2_START;
		while (!TIMEOUT::triggered() && !(i2c->ISR & I2C_ISR_TXIS));
		i2c->TXDR = reg;
		while (!TIMEOUT::triggered() && !(i2c->ISR & I2C_ISR_TC));

		i2c->CR2 = I2C_CR2_AUTOEND | (1 << 16) | (slave_addr << 1) | I2C_CR2_RD_WRN | I2C_CR2_START;
		while (!TIMEOUT::triggered() && !(i2c->ISR & I2C_ISR_RXNE));
		r = i2c->RXDR;
		i2c->CR2 = I2C_CR2_STOP;
		return r;
	}

	static void read_reg(const uint8_t reg, uint8_t *data, uint16_t length) {
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void transfer(const uint8_t *data, int length, bool direction, bool restart = false) {
		if (direction) write<TIMEOUT>(data, length, restart);
		else read<TIMEOUT>(const_cast<uint8_t *>(data), length);
	}
};

template<typename SYSCLK, const uint32_t INSTANCE, const uint8_t SLAVE_ADDR, const uint32_t FREQUENCY>
uint8_t MASTER<SYSCLK, INSTANCE, SLAVE_ADDR, FREQUENCY>::slave_addr;

typedef void(*ADDR_CALLBACK)(uint8_t addr);
typedef uint8_t(*TX_CALLBACK)(void);
typedef void(*RX_CALLBACK)(uint8_t data);

template<typename SYSCLK,
	const uint32_t INSTANCE,
	const uint8_t OWN_ADDRESS,
	const uint8_t OWN_ADDRESS2,
	const uint8_t OWN_ADDRESS2_MASK,
	ADDR_CALLBACK addr_callback,
	RX_CALLBACK rx_callback,
	TX_CALLBACK tx_callback>
struct SLAVE {
	static bool transmitting;
	static constexpr I2C_TypeDef *i2c = (INSTANCE == 1 ? I2C1 : I2C2);

	static void init(void) {
		i2c->CR1 &= ~I2C_CR1_PE;
		RCC->APB1ENR |= (INSTANCE == 1 ? RCC_APB1ENR_I2C1EN : RCC_APB1ENR_I2C2EN);
		i2c->TIMINGR = 0xb0420f13;
		i2c->CR1 = I2C_CR1_PE | I2C_CR1_ADDRIE | I2C_CR1_RXIE;
		i2c->OAR1 = (OWN_ADDRESS << 1) | I2C_OAR1_OA1EN;
		if (OWN_ADDRESS2 != 0 || OWN_ADDRESS2_MASK != 0) {
			i2c->OAR2 = (OWN_ADDRESS2 << 1) | (OWN_ADDRESS2_MASK << 8) | I2C_OAR2_OA2EN;
		}
		NVIC_SetPriority(INSTANCE == 1 ? I2C1_IRQn: I2C2_IRQn, 0);
		NVIC_EnableIRQ(INSTANCE == 1 ? I2C1_IRQn: I2C2_IRQn);
		transmitting = false;
	}

	static bool handle_irq(void) {
		uint32_t isr = i2c->ISR;
		uint32_t data = i2c->RXDR;

		if (isr & I2C_ISR_ADDR) {
			addr_callback(isr >> 16);
			i2c->ICR |= I2C_ICR_ADDRCF;
			if (isr & I2C_ISR_DIR) {
				transmitting = true;
				i2c->CR1 |= I2C_CR1_TXIE;
			} else {
				transmitting = false;
			}
		}
		if (!transmitting && (isr & I2C_ISR_RXNE)) {
			rx_callback(data);
		}
		if (transmitting && (isr & I2C_ISR_TXIS)) {
			i2c->CR1 &= ~I2C_CR1_TXIE;
			i2c->TXDR = tx_callback();
		}
		if (isr & I2C_ISR_STOPF) {
			i2c->ICR |= I2C_ICR_STOPCF;
		}
		return true;
	}

};

template<typename SYSCLK, const uint32_t INSTANCE, const uint8_t OWN_ADDRESS, const uint8_t OWN_ADDRESS2, const uint8_t OWN_ADDRESS2_MASK,
	ADDR_CALLBACK addr_callback, RX_CALLBACK rx_callback, TX_CALLBACK tx_callback>
bool SLAVE<SYSCLK, INSTANCE, OWN_ADDRESS, OWN_ADDRESS2, OWN_ADDRESS2_MASK, addr_callback, rx_callback, tx_callback>::transmitting;

};
