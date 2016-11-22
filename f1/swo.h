#ifndef SWO_H
#define SWO_H

#include <arm_etm.h>

namespace SWO
{

struct T {
	static bool trace_enabled;

	static void init(void) {
		if (!(CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) ||
				!(DBGMCU->CR & DBGMCU_CR_TRACE_IOEN)) {
			// Some (all?) STM32s don't allow writes to DBGMCU register until
			// C_DEBUGEN in CoreDebug->DHCSR is set. This cannot be set by the
			// CPU itself, so in practice you need to connect to the CPU with
			// a debugger once before resetting it.
			trace_enabled = false;
			return;
		}

		/* STM32 specific configuration to enable the TRACESWO IO pin */
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
		AFIO->MAPR |= (2 << 24); // Disable JTAG to release TRACESWO
		DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN; // Enable IO trace pins

		/* Configure Trace Port Interface Unit */
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable access to registers
		TPI->ACPR = 0; // Trace clock = HCLK/(x+1) = 8MHz
		TPI->SPPR = 2; // Pin protocol = NRZ/USART
		TPI->FFCR = 0x102; // TPIU packet framing enabled when bit 2 is set.
		// You can use 0x100 if you only need DWT/ITM and not ETM.

		/* Configure instrumentation trace macroblock */
		ITM->LAR = 0xC5ACCE55;
		ITM->TCR = (1 << ITM_TCR_TraceBusID_Pos) // Trace bus ID for TPIU
			| (1 << ITM_TCR_SYNCENA_Pos) // Enable sync packets
			| (1 << ITM_TCR_ITMENA_Pos); // Main enable for ITM
		ITM->TER = 0xFFFFFFFF; // Enable all stimulus ports
		trace_enabled = true;
	}

	static bool enabled(void) {
		return trace_enabled;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void putc(char data) {
			while (ITM->PORT[0].u32 == 0);
			ITM->PORT[0].u8 = (uint8_t) data;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void puts(const char *data) {
		while (*data) {
			while (ITM->PORT[0].u32 == 0);
			ITM->PORT[0].u8 = (uint8_t) *data++;
		}
	}
};

bool T::trace_enabled;

}


#endif
