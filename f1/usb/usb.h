#pragma once

#include <stdlib.h>
#include <stm32f1xx.h>
#include <utils.h>
#include <usb/definitions.h>
#include <usb/endpoint.h>

namespace USB_T {

typedef bool(*IRQ_CALLBACK)(void);
typedef bool(*HANDLER_FUNCTION)(uint8_t ep, bool out);

static bool default_irq_callback(void) { return true; }
static bool invalid_ep_ctr(uint8_t ep, bool out) {
	while (1);
}

template<DESCRIPTOR_CALLBACK get_descriptor_callback,
	SETUP_CALLBACK handle_setup_callback,
	IRQ_CALLBACK irq_callback,
	typename EP1_HANDLER = EP_DEFAULT_HANDLER,
	typename EP2_HANDLER = EP_DEFAULT_HANDLER,
	typename EP3_HANDLER = EP_DEFAULT_HANDLER,
	typename EP4_HANDLER = EP_DEFAULT_HANDLER,
	typename EP5_HANDLER = EP_DEFAULT_HANDLER,
	typename EP6_HANDLER = EP_DEFAULT_HANDLER,
	typename EP7_HANDLER = EP_DEFAULT_HANDLER>
struct T {
	typedef EP0_HANDLER_T<get_descriptor_callback, handle_setup_callback> EP0_HANDLER;

	static constexpr HANDLER_FUNCTION ep_to_handler(const uint8_t ep) {
		return ep == EP1_HANDLER::number ? EP1_HANDLER::ctr :
			ep == EP2_HANDLER::number ? EP2_HANDLER::ctr :
			ep == EP3_HANDLER::number ? EP3_HANDLER::ctr :
			ep == EP4_HANDLER::number ? EP4_HANDLER::ctr :
			ep == EP5_HANDLER::number ? EP5_HANDLER::ctr :
			ep == EP6_HANDLER::number ? EP6_HANDLER::ctr :
			ep == EP7_HANDLER::number ? EP7_HANDLER::ctr : invalid_ep_ctr;
	}

	static constexpr HANDLER_FUNCTION ep_handler[8] {
		EP0_HANDLER::ctr, ep_to_handler(1), ep_to_handler(2), ep_to_handler(3),
		ep_to_handler(4), ep_to_handler(5), ep_to_handler(6), ep_to_handler(7),
	};

	static void init(void) {
		RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
		GPIOA->CRH = (GPIOA->CRH & 0xfff0ffff) | 0x00010000;
		GPIOA->BRR = 1 << 12;
		for (volatile int i = 0; i < 1000; i++);
		GPIOA->CRH |= 0x00040000;
		if (((RCC->CFGR & RCC_CFGR_PLLMULL_Msk) >> RCC_CFGR_PLLMULL_Pos) >= 4) {
			RCC->CFGR &= ~RCC_CFGR_USBPRE;
		} else {
			RCC->CFGR |= RCC_CFGR_USBPRE;
		}

		RCC->APB1RSTR |= RCC_APB1RSTR_USBRST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;

		RCC->APB1ENR |= RCC_APB1ENR_USBEN;
		USB->CNTR = USB_CNTR_FRES;
		USB->CNTR = 0;
		USB->ISTR = 0;
		enable_irq();
	}

	static void enable_irq(void) {
		NVIC_ClearPendingIRQ(USB_HP_CAN1_TX_IRQn);
		NVIC_ClearPendingIRQ(USBWakeUp_IRQn);
		NVIC_ClearPendingIRQ(USB_LP_CAN1_RX0_IRQn);
		NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
		NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
		NVIC_EnableIRQ(USBWakeUp_IRQn);
		USB->CNTR |= USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM;
	}

	static void disable_irq(void) {
		USB->CNTR &= ~(USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM);
		NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
		NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
		NVIC_DisableIRQ(USBWakeUp_IRQn);
	}

	static bool handle_irq(void) {
		bool exit_idle = false;
		if (irq_callback != default_irq_callback) {
			exit_idle = irq_callback();
		}
		if (USB->ISTR & USB_ISTR_CTR) {
			exit_idle = ctr();
			USB->ISTR &= ~USB_ISTR_CTR;
		}
		if (USB->ISTR & USB_ISTR_RESET) {
			reset();
			USB->ISTR &= ~USB_ISTR_RESET;
		}
		if (USB->ISTR & USB_ISTR_SUSP) {
			suspend();
			USB->ISTR &= ~USB_ISTR_SUSP;
		}
		if (USB->ISTR & USB_ISTR_WKUP) {
			wake();
			USB->ISTR &= ~USB_ISTR_WKUP;
		}
		return exit_idle;
	}

	static bool handle_hp_irq(void) {
		bool exit_idle = false;
		if (USB->ISTR & USB_ISTR_CTR) {
			exit_idle = ctr();
			USB->ISTR &= ~USB_ISTR_CTR;
		}
		return exit_idle;
	}

	static void reset(void) {
		uint16_t btable_offset = 64;
		btable_offset = EP0_HANDLER::init(btable_offset);
		btable_offset = EP1_HANDLER::init(btable_offset);
		btable_offset = EP2_HANDLER::init(btable_offset);
		btable_offset = EP3_HANDLER::init(btable_offset);
		btable_offset = EP4_HANDLER::init(btable_offset);
		btable_offset = EP5_HANDLER::init(btable_offset);
		btable_offset = EP6_HANDLER::init(btable_offset);
		EP7_HANDLER::init(btable_offset);
		USB->DADDR = 0x80;
	}

	static void suspend(void) {
		USB->CNTR |= USB_CNTR_LP_MODE | USB_CNTR_FSUSP;
	}

	static void wake(void) {
		USB->CNTR &= ~USB_CNTR_LP_MODE;
		USB->CNTR = USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM;
	}

	static bool ctr(void) {
		uint8_t ep = USB->ISTR & USB_ISTR_EP_ID;
		bool out = USB->ISTR & USB_ISTR_DIR;
		return ep_handler[ep](ep, out);
	}

	static void enable(void) {
	}

	static void disable(void) {
	}
};

template<DESCRIPTOR_CALLBACK get_descriptor_callback,
	SETUP_CALLBACK handle_setup_callback,
	IRQ_CALLBACK irq_callback,
	typename EP1_HANDLER, typename EP2_HANDLER, typename EP3_HANDLER, typename EP4_HANDLER,
	typename EP5_HANDLER, typename EP6_HANDLER, typename EP7_HANDLER>
constexpr HANDLER_FUNCTION T<get_descriptor_callback, handle_setup_callback, irq_callback,
	  EP1_HANDLER, EP2_HANDLER, EP3_HANDLER, EP4_HANDLER, EP5_HANDLER, EP6_HANDLER, EP7_HANDLER>::ep_handler[8];

}
