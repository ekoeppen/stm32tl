#ifndef __SOFT_RTC_H
#define __SOFT_RTC_H

#include <clocks.h>

struct DATE_TIME {
	static const uint8_t month_length[12];

	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;

	void increment(void) {
		seconds++;
		if (seconds > 59) {
			seconds = 0;
			minutes++;
			if (minutes > 59) {
				minutes = 0;
				hours++;
				if (hours > 23) {
					hours = 0;
					day++;
					if (day > (month == 1 && (year & 0x3) == 0 ? 29 : month_length[month])) {
						day = 0;
						month++;
						if (month > 11) {
							month = 0;
							year++;
						}
					}
				}
			}
		}
	}
};

const uint8_t DATE_TIME::month_length[12] = {31, 30, 28, 30, 31, 30, 31, 31, 30, 31, 30, 31};

template<typename CLOCK>
struct SOFT_RTC_T {
	static volatile uint16_t tick;
	static DATE_TIME date_time;

	static void init(void) {
		tick = CLOCK::frequency;
		CLOCK::claim();
	};

	static void update(void) {
		tick--;
		if (tick == 0) {
			date_time.increment();
			tick = CLOCK::frequency;
		}
	};

	static inline void disable(void) {
		CLOCK::release();
	}

	static void get(DATE_TIME& dst) {
		CLOCK::disable_irq();
		dst.year = date_time.year;
		dst.month = date_time.month;
		dst.day = date_time.day;
		dst.hours = date_time.hours;
		dst.minutes = date_time.minutes;
		dst.seconds = date_time.seconds;
		CLOCK::enable_irq();
	}

	static void set(DATE_TIME& src) {
		CLOCK::disable_irq();
		date_time.year = src.year;
		date_time.month = src.month;
		date_time.day = src.day;
		date_time.hours = src.hours;
		date_time.minutes = src.minutes;
		date_time.seconds = src.seconds;
		CLOCK::enable_irq();
	}
};

template<typename CLOCK>
volatile uint16_t SOFT_RTC_T<CLOCK>::tick;

template<typename CLOCK>
DATE_TIME SOFT_RTC_T<CLOCK>::date_time;

#endif

