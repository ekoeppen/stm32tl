#ifndef __RTC_H
#define __RTC_H

#include <stm32f4xx.h>
#include <rtc.h>

enum {
	RTC_CLOCK_LSE = RCC_BDCR_RTCSEL_LSE,
	RTC_CLOCK_HSE = RCC_BDCR_RTCSEL_HSE,
	RTC_CLOCK_LSI = RCC_BDCR_RTCSEL_LSI
};

template<const int CLOCK = RTC_CLOCK_LSI, const bool BYPASS_SHADOW = false>
struct RTC_T {
	volatile static bool irq_triggered;

	static void init(uint32_t rtc_clock) {
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_DBP;
		RCC->BDCR |= CLOCK | RCC_BDCR_RTCEN;
		unlock();
		if (CLOCK == RTC_CLOCK_LSI) {
			RCC->CSR |= RCC_CSR_LSION;
			while (!RCC->CSR & RCC_CSR_LSIRDY);
		}
		enter_init_mode();
		update_prescaler(rtc_clock);
		if (BYPASS_SHADOW) {
			RTC->CR |= RTC_CR_BYPSHAD;
		} else {
			RTC->CR &= ~RTC_CR_BYPSHAD;
		}
	}

	static void run(void) {
		RTC->ISR &= ~RTC_ISR_INIT;
		lock();
	}

	static void enter_init_mode(void) {
		unlock();
		RTC->ISR |= RTC_ISR_INIT;
		while (!(RTC->ISR & RTC_ISR_INITF));
	}

	static void update_prescaler(uint32_t rtc_clock) {
		RTC->PRER = (63 << 16) | ((rtc_clock / 64) - 1);
	}

	static void unlock(void) {
		RTC->WPR = 0xca;
		RTC->WPR = 0x53;
	}

	static void lock(void) {
		RTC->WPR = 0xff;
	}

	static void wait_for_sync(void) {
		if ((RTC->CR & RTC_CR_BYPSHAD) == 0) {
			RTC->ISR &= ~RTC_ISR_RSF;
			while (!(RTC->ISR & RTC_ISR_RSF));
		}
	}

	static void disable_alarm(void) {
		RTC->CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRAIE);
		while (!(RTC->ISR & RTC_ISR_ALRAWF));
	}

	static void enable_alarm(void) {
		RTC->ISR &= ~RTC_ISR_ALRAF;
		RTC->CR |= RTC_CR_ALRAE | RTC_CR_ALRAIE;
		NVIC_ClearPendingIRQ(RTC_IRQn);
		NVIC_EnableIRQ(RTC_IRQn);
	}

	static void set_time_date(char *date_time) {
		for (char *d = date_time; *d; d++) *d -= '0';
		RTC->DR =
			date_time[2] << 20 | date_time[3] << 16 |
			date_time[4] << 12 | date_time[5] << 8 |
			date_time[6] << 4 | date_time[7] |
			date_time[8] << 13;
		RTC->TR =
			date_time[9] << 20 | date_time[10] << 16 |
			date_time[11] << 12 | date_time[12] << 8 |
			date_time[13] << 4 | date_time[14];
		delay(1000);
	}

	static char *get_time_date(void) {
		static char buffer[15];
		uint32_t dr;
		uint32_t tr;

		wait_for_sync();
		dr = RTC->DR;
		tr = RTC->TR;

		buffer[0] = 2; buffer[1] = 0;
		buffer[2] = (dr >> 20) & 0xf; buffer[3] = (dr >> 16) & 0xf;
		buffer[4] = (dr >> 12) & 0x1; buffer[5] = (dr >> 8) & 0xf;
		buffer[6] = (dr >> 4) & 0x3; buffer[7] = dr & 0xf;

		buffer[8] = (tr >> 20) & 0x3; buffer[9] = (tr >> 16) & 0xf;
		buffer[10] = (tr >> 12) & 0x7; buffer[11] = (tr >> 8) & 0xf;
		buffer[12] = (tr >> 4) & 0x7; buffer[13] = tr & 0xf;
		buffer[14] = 0;

		for (int i = 0; i < sizeof(buffer) - 1; i++) buffer[i] += '0';
		return buffer;
	}

	static uint32_t set_alarm_minutes(int delta_minutes) {
		uint32_t hours, minutes;
		uint32_t tr;
		uint32_t alarmr;

		wait_for_sync();
		disable_alarm();
		tr = RTC->TR;
		hours = ((tr >> 20) & 0x3) * 10 + ((tr >> 16) & 0xf);
		minutes = ((tr >> 12) & 0x7) * 10 + ((tr >> 8) & 0xf);
		minutes += delta_minutes;
		alarmr = RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4;
		if (minutes >= 60) {
			minutes -= 60;
			hours++;
			alarmr &= ~RTC_ALRMAR_MSK3;
			if (hours >= 24) {
				hours = 0;
			}
			alarmr |= ((hours / 10) << 20) | ((hours % 10) << 16);
		}
		alarmr |= ((minutes / 10) << 12) | ((minutes % 10) << 8);
		RTC->ALRMAR = alarmr;
		enable_alarm();
		return alarmr;
	}

	static int set_alarm_seconds(int delta_seconds) {
		uint32_t hours, minutes, seconds;
		uint32_t tr;
		uint32_t alarmr;

		wait_for_sync();
		disable_alarm();
		tr = RTC->TR;
		hours = ((tr >> 20) & 0x3) * 10 + ((tr >> 16) & 0xf);
		minutes = ((tr >> 12) & 0x7) * 10 + ((tr >> 8) & 0xf);
		seconds = ((tr >> 4) & 0x7) * 10 + (tr & 0xf);
		seconds += delta_seconds;
		alarmr = RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4;
		if (seconds >= 60) {
			seconds -= 60;
			minutes++;
			alarmr &= ~RTC_ALRMAR_MSK2;
			if (minutes == 60) {
				minutes = 0;
				hours++;
				alarmr &= ~RTC_ALRMAR_MSK3;
				if (hours == 24) {
					hours = 0;
				}
				alarmr |= ((hours / 10) << 20) | ((hours % 10) << 16);
			}
			alarmr |= ((minutes / 10) << 12) | ((minutes % 10) << 8);
		}
		alarmr |= ((seconds / 10) << 4) | (seconds % 10);
		RTC->ALRMAR = alarmr;
		enable_alarm();
		return alarmr;
	}

	static int set_alarm_per_minute(void) {
		wait_for_sync();
		disable_alarm();
		RTC->ALRMAR= RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4;
		enable_alarm();
		return RTC->ALRMAR;
	}

	static void set_alarm(const char *time) {
		uint32_t alarmr = 0;

		if (time[0] |= '.') {
			alarmr |= RTC_ALRMAR_MSK3 | ((time[0] - '0') << 20) | ((time[1] - '1') << 16);
		}

		if (time[2] != '.') {
			alarmr |= RTC_ALRMAR_MSK2 | ((time[0] - '2') << 12) | ((time[1] - '3') << 8);
		}

		if (time[4] |= '.') {
			alarmr |= RTC_ALRMAR_MSK1 | ((time[0] - '4') << 4) | ((time[1] - '5') << 0);
		}
		RTC->ALRMAR = alarmr;
	}

	static constexpr int exti_source(void) {
		return 1 << 17;
	}

	static constexpr int exti_trigger_falling(void) {
		return 0;
	}

	static constexpr int exti_trigger_rising(void) {
		return 1 << 17;
	}

	static constexpr int exti_syscfg(const int i) {
		return 0;
	}
};

template<const int CLOCK, const bool BYPASS_SHADOW>
volatile bool RTC_T<CLOCK, BYPASS_SHADOW>::irq_triggered;


#endif
