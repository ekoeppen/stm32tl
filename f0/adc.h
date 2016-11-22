#pragma once

namespace ADC_T {

enum {
	ADC_IN0 = 0,
	ADC_IN1,
	ADC_IN2,
	ADC_IN3,
	ADC_IN4,
	ADC_IN5,
	ADC_IN6,
	ADC_IN7,
	ADC_IN8,
	ADC_IN9,
	ADC_IN10,
	ADC_IN11,
	ADC_IN12,
	ADC_IN13,
	ADC_IN14,
	ADC_IN15,
	ADC_IN16,
	ADC_IN17
};

struct SINGLE_POLLED {
	static void init(void) {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		ADC1->CR |= ADC_CR_ADEN;
	}

	static void disable(void) {
		ADC1->CR |= ADC_CR_ADDIS;
		while (ADC1->CR & ADC_CR_ADDIS);
		RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
	}

	template <typename SOURCE>
	static uint32_t sample(void) {
		ADC1->CHSELR = (1 << SOURCE::adc_channel());
		ADC1->SMPR = ADC_SMPR1_SMPR_2;
		ADC1->CR |= ADC_CR_ADSTART;
		while (!end_of_conversion());
		return ADC1->DR;
	}

	static uint32_t sample(uint32_t channel) {
		ADC1->CHSELR = (1 << channel);
		ADC1->SMPR = ADC_SMPR1_SMPR_2;
		ADC1->CR |= ADC_CR_ADSTART;
		while (!end_of_conversion());
		return ADC1->DR;
	}

	static bool end_of_conversion(void) {
		return (ADC1->ISR & ADC_ISR_EOC) != 0;
	}
};

template<typename CHANNEL0,
	typename CHANNEL1 = PIN_UNUSED,
	typename CHANNEL2 = PIN_UNUSED,
	typename CHANNEL3 = PIN_UNUSED,
	typename CHANNEL4 = PIN_UNUSED,
	typename CHANNEL5 = PIN_UNUSED,
	typename CHANNEL6 = PIN_UNUSED,
	typename CHANNEL7 = PIN_UNUSED,
	typename CHANNEL8 = PIN_UNUSED,
	typename CHANNEL9 = PIN_UNUSED,
	typename CHANNEL10 = PIN_UNUSED,
	typename CHANNEL11 = PIN_UNUSED,
	typename CHANNEL12 = PIN_UNUSED,
	typename CHANNEL13 = PIN_UNUSED,
	typename CHANNEL14 = PIN_UNUSED,
	typename CHANNEL15 = PIN_UNUSED,
	typename CHANNEL16 = PIN_UNUSED,
	typename CHANNEL17 = PIN_UNUSED,
	typename CHANNEL18 = PIN_UNUSED>
struct CONTINUOUS {
	static uint16_t *samples;
	static uint32_t active_channel;
	static bool once;
	static volatile bool complete;

	static void init(void) {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		ADC1->CR = ADC_CR_ADEN;
		ADC1->CFGR1 = ADC_CFGR1_CONT;
		ADC1->IER = ADC_IER_EOCIE | ADC_IER_EOSEQIE;
		ADC1->CHSELR =
			CHANNEL0::adc_channel_bit | CHANNEL1::adc_channel_bit | CHANNEL2::adc_channel_bit |
			CHANNEL3::adc_channel_bit | CHANNEL4::adc_channel_bit | CHANNEL5::adc_channel_bit |
			CHANNEL6::adc_channel_bit | CHANNEL7::adc_channel_bit | CHANNEL8::adc_channel_bit |
			CHANNEL9::adc_channel_bit | CHANNEL10::adc_channel_bit | CHANNEL11::adc_channel_bit |
			CHANNEL12::adc_channel_bit | CHANNEL13::adc_channel_bit | CHANNEL14::adc_channel_bit |
			CHANNEL15::adc_channel_bit | CHANNEL16::adc_channel_bit | CHANNEL17::adc_channel_bit |
			CHANNEL18::adc_channel_bit;
#ifdef STM32F030
		NVIC_EnableIRQ(ADC1_IRQn);
#else
		NVIC_EnableIRQ(ADC1_COMP_IRQn);
#endif

	}

	static void disable(void) {
		ADC1->CR |= ADC_CR_ADDIS;
		while (ADC1->CR & ADC_CR_ADDIS);
		RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void sample(uint16_t *sample_buffer, bool sequence_once) {
		ADC1->SMPR = ADC_SMPR_SMP_2;
		once = sequence_once;
		samples = sample_buffer;
		active_channel = 0;
		complete = false;
		ADC1->CR |= ADC_CR_ADSTART;
		while (!TIMEOUT::triggered() && !complete) {
			enter_idle();
		}
	}

	static bool end_of_conversion(void) {
		return (ADC1->ISR & ADC_ISR_EOC) != 0;
	}

	static bool handle_irq(void) {
		bool r = false;
		if (ADC1->ISR & ADC_ISR_EOC) {
			samples[active_channel++] = ADC1->DR;
		}
		if (ADC1->ISR & ADC_ISR_EOSEQ) {
			ADC1->ISR |= ADC_ISR_EOSEQ;
			if (once) {
				ADC1->CR |= ADC_CR_ADSTP;
				complete = true;
				r = true;
			} else {
				active_channel = 0;
			}
		}
		return r;
	}
};

template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
uint16_t* CONTINUOUS<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::samples;
template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
uint32_t CONTINUOUS<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::active_channel;
template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
bool CONTINUOUS<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::once;
template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
volatile bool CONTINUOUS<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::complete;

template<typename CHANNEL0,
	typename CHANNEL1 = PIN_UNUSED,
	typename CHANNEL2 = PIN_UNUSED,
	typename CHANNEL3 = PIN_UNUSED,
	typename CHANNEL4 = PIN_UNUSED,
	typename CHANNEL5 = PIN_UNUSED,
	typename CHANNEL6 = PIN_UNUSED,
	typename CHANNEL7 = PIN_UNUSED,
	typename CHANNEL8 = PIN_UNUSED,
	typename CHANNEL9 = PIN_UNUSED,
	typename CHANNEL10 = PIN_UNUSED,
	typename CHANNEL11 = PIN_UNUSED,
	typename CHANNEL12 = PIN_UNUSED,
	typename CHANNEL13 = PIN_UNUSED,
	typename CHANNEL14 = PIN_UNUSED,
	typename CHANNEL15 = PIN_UNUSED,
	typename CHANNEL16 = PIN_UNUSED,
	typename CHANNEL17 = PIN_UNUSED,
	typename CHANNEL18 = PIN_UNUSED>
struct CONTINUOUS_DMA {
	static uint16_t *samples;
	static uint32_t active_channel;
	static bool once;
	static volatile bool complete;

	static constexpr uint32_t channel_count =
		!CHANNEL0::is_unused + !CHANNEL1::is_unused + !CHANNEL2::is_unused +
		!CHANNEL3::is_unused + !CHANNEL4::is_unused + !CHANNEL5::is_unused +
		!CHANNEL6::is_unused + !CHANNEL7::is_unused + !CHANNEL8::is_unused +
		!CHANNEL9::is_unused + !CHANNEL10::is_unused + !CHANNEL11::is_unused +
		!CHANNEL12::is_unused + !CHANNEL13::is_unused + !CHANNEL14::is_unused +
		!CHANNEL15::is_unused + !CHANNEL16::is_unused + !CHANNEL17::is_unused +
		!CHANNEL18::is_unused;

	static void init(void) {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		RCC->AHBENR |= RCC_AHBENR_DMAEN;
		ADC1->CR = ADC_CR_ADEN;
		ADC1->CFGR1 = ADC_CFGR1_CONT | ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
		ADC1->IER = ADC_IER_EOCIE | ADC_IER_EOSEQIE;
		ADC1->SMPR = ADC_SMPR_SMP_2;
		ADC1->CHSELR =
			CHANNEL0::adc_channel_bit | CHANNEL1::adc_channel_bit | CHANNEL2::adc_channel_bit |
			CHANNEL3::adc_channel_bit | CHANNEL4::adc_channel_bit | CHANNEL5::adc_channel_bit |
			CHANNEL6::adc_channel_bit | CHANNEL7::adc_channel_bit | CHANNEL8::adc_channel_bit |
			CHANNEL9::adc_channel_bit | CHANNEL10::adc_channel_bit | CHANNEL11::adc_channel_bit |
			CHANNEL12::adc_channel_bit | CHANNEL13::adc_channel_bit | CHANNEL14::adc_channel_bit |
			CHANNEL15::adc_channel_bit | CHANNEL16::adc_channel_bit | CHANNEL17::adc_channel_bit |
			CHANNEL18::adc_channel_bit;

	}

	static void disable(void) {
		ADC1->CR |= ADC_CR_ADDIS;
		while (ADC1->CR & ADC_CR_ADDIS);
		RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
		RCC->AHBENR &= ~RCC_AHBENR_DMAEN;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void sample(uint16_t *sample_buffer, bool sequence_once) {
		DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;
		DMA1_Channel1->CMAR = (uint32_t) sample_buffer;
		DMA1_Channel1->CNDTR = channel_count;
		DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_TEIE | DMA_CCR_CIRC;
		DMA1_Channel1->CCR |= DMA_CCR_EN;
		complete = false;
		ADC1->CR |= ADC_CR_ADSTART;
		while (!TIMEOUT::triggered() && !complete) {
			enter_idle();
		}
	}

	static bool end_of_conversion(void) {
		return (ADC1->ISR & ADC_ISR_EOC) != 0;
	}

	static bool handle_irq(void) {
		bool r = false;
		return r;
	}
};

template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
uint16_t* CONTINUOUS_DMA<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::samples;
template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
uint32_t CONTINUOUS_DMA<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::active_channel;
template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
bool CONTINUOUS_DMA<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::once;
template<typename CHANNEL0, typename CHANNEL1, typename CHANNEL2,
	typename CHANNEL3, typename CHANNEL4, typename CHANNEL5,
	typename CHANNEL6, typename CHANNEL7, typename CHANNEL8,
	typename CHANNEL9, typename CHANNEL10, typename CHANNEL11,
	typename CHANNEL12, typename CHANNEL13, typename CHANNEL14,
	typename CHANNEL15, typename CHANNEL16, typename CHANNEL17,
	typename CHANNEL18>
volatile bool CONTINUOUS_DMA<CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, CHANNEL8,
	CHANNEL9, CHANNEL10, CHANNEL11, CHANNEL12, CHANNEL13, CHANNEL14, CHANNEL15, CHANNEL16, CHANNEL17,
	CHANNEL18>::complete;

}

