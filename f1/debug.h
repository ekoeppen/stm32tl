#ifndef DEBUG_H
#define DEBUG_H

template<const bool ENABLED = false>
struct DEBUG_T {
	static void init(void) {
		if (ENABLED) {
			enable();
		}
	}

	static void enable(void) {
		DBGMCU->CR |= (DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP);
	}

	static void disabe(void) {
		DBGMCU->CR &= ~(DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP);
	}
};

#endif
