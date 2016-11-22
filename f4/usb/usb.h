#pragma once

#include <stm32f4xx.h>
#include <usb/definitions.h>
#include <usb/endpoint.h>
#include <utils.h>

namespace USB_T {

#define HANDLE_INT(x) if (status & x) { handle_##x(); status &= ~x; }

struct EP_DEFAULT_HANDLER {
	static void init(void) { }
	static void handle_in(void) { }
	static void handle_out(void) { }
	static constexpr uint32_t INTMASK = 0;
};

template<DESCRIPTOR_CALLBACK get_descriptor_callback,
	SETUP_CALLBACK setup_callback,
	typename EP1 = EP_DEFAULT_HANDLER,
	typename EP2 = EP_DEFAULT_HANDLER,
	typename EP3 = EP_DEFAULT_HANDLER>
struct CORE {
	typedef EP0_HANDLER<get_descriptor_callback, setup_callback> EP0;

	static void INT_Enable(void) {
		// TODO
	}

	static void INT_Disable(void) {
		// TODO
	}

	static void init(void) {
		uint16_t start, depth;

		USBG->GAHBCFG &= ~USB_GAHBCFG_GLBLINTRMSK;

		/* Core Soft Reset */
		USBG->GRSTCTL |= USB_GRSTCTL_CSFTRST;
		while (USBG->GRSTCTL & USB_GRSTCTL_CSFTRST) {}

		for (volatile int i = 0; i < 1000; i++);

		/* Wait for AHB master IDLE state. */
		while (!(USBG->GRSTCTL & USB_GRSTCTL_AHBIDLE)) {}

		INT_Enable();
		for (volatile int i = 0; i < 100000; i++);
		INT_Disable();

		/* Stall on non-zero len status OUT packets (ctrl transfers). */
		USBD->DCFG |= USB_DCFG_NZSTSOUTHSHK;
		USBD->DCFG = (USBD->DCFG & ~_USB_DCFG_DEVSPD_MASK) | 3; /* Full speed PHY */

		/* Set periodic frame interval to 80% */
		USBD->DCFG &= ~_USB_DCFG_PERFRINT_MASK;

		USBG->GAHBCFG = (USBG->GAHBCFG & ~_USB_GAHBCFG_HBSTLEN_MASK) |
			USB_GAHBCFG_DMAEN | USB_GAHBCFG_HBSTLEN_INCR;

		/* Ignore frame numbers on ISO transfers. */
		USBD->DCTL = (USBD->DCTL & ~DCTL_WO_BITMASK) | USB_DCTL_IGNRFRMNUM;

		/* Set Rx FIFO size */
		start = 0;
		depth = 64;
		USBG->GRXFSIZ = (depth << _USB_GRXFSIZ_RXFDEP_SHIFT) & _USB_GRXFSIZ_RXFDEP_MASK;

		/* Set Tx EP0 FIFO size */
		start += depth;
		depth = 16;
		USBG->DIEPTXF0_HNPTXFSIZ =
			((depth << _USB_GNPTXFSIZ_NPTXFINEPTXF0DEP_SHIFT) & _USB_GNPTXFSIZ_NPTXFINEPTXF0DEP_MASK) |
			((start << _USB_GNPTXFSIZ_NPTXFSTADDR_SHIFT) & _USB_GNPTXFSIZ_NPTXFSTADDR_MASK);

		/* Set Tx EP1 FIFO size */
		start += depth;
		depth = 16;
		USBG->DIEPTXF[1] =
			((depth << _USB_DIEPTXF1_INEPNTXFDEP_SHIFT) & _USB_DIEPTXF1_INEPNTXFDEP_MASK) |
			((start << _USB_DIEPTXF1_INEPNTXFSTADDR_SHIFT) & _USB_DIEPTXF1_INEPNTXFSTADDR_MASK);

		USBG->GRSTCTL = USB_GRSTCTL_TXFFLSH | (0x10 << _USB_GRSTCTL_TXFNUM_SHIFT);
		while (USBG->GRSTCTL & USB_GRSTCTL_TXFFLSH) {}

		USBG->GRSTCTL = USB_GRSTCTL_RXFFLSH;
		while (USBG->GRSTCTL & USB_GRSTCTL_RXFFLSH) {}

		USBD->DIEPMSK  = 0;
		USBD->DOEPMSK  = 0;
		USBD->DAINTMSK = 0;
		USBD->DIEPEMPMSK = 0;

		for (int i = 0; i < 3; i++) {
			USBDIEP[i]->DIEPCTL  = 0;
			USBDIEP[i]->DIEPTSIZ = 0;
			USBDIEP[i]->DIEPINT  = 0xFFFFFFFF;
		}

		for (int i = 0; i < 3; i++) {
			USBDOEP[i]->DOEPCTL  = 0;
			USBDOEP[i]->DOEPTSIZ = 0;
			USBDOEP[i]->DOEPINT  = 0xFFFFFFFF;
		}

		USBG->GINTMSK = 1;

		USBG->GINTMSK = USB_GINTMSK_USBRSTMSK | USB_GINTMSK_USBSUSPMSK | USB_GINTMSK_OTGINTMSK | 1;

		USBG->GAHBCFG |= USB_GAHBCFG_GLBLINTRMSK;

		NVIC_ClearPendingIRQ(OTG_FS_IRQn);
		NVIC_EnableIRQ(OTG_FS_IRQn);
		INT_Enable();

		USBG->GCCFG |= (1 << 19);
		/*
		USBD->DCTL = ( USBD->DCTL & ~DCTL_WO_BITMASK ) | USB_DCTL_SFTDISCON;
		for (volatile int i = 0; i < 10000; i++);
		USBD->DCTL &= ~( DCTL_WO_BITMASK | USB_DCTL_SFTDISCON );
		*/
	}

	static void handle_USB_GINTSTS_ENUMDONE(void) {
		USBG->GINTMSK = 0;
		USBG->GINTSTS = 0xFFFFFFFF;
		USBG->GINTMSK =
			USB_GINTMSK_USBSUSPMSK     |
			USB_GINTMSK_USBRSTMSK      |
			USB_GINTMSK_ENUMDONEMSK    |
			USB_GINTMSK_IEPINTMSK      |
			USB_GINTMSK_OEPINTMSK      |
			USB_GINTMSK_WKUPINTMSK;
	}

	static void handle_USB_GINTSTS_RESETDET(void) {
		USBG->GINTSTS = USB_GINTSTS_RESETDET;
	}

	static void handle_USB_GINTSTS_SOF(void) {
	}

	static void handle_USB_GINTSTS_USBRST(void) {
		USBD->DCTL &= ~(DCTL_WO_BITMASK | USB_DCTL_RMTWKUPSIG);
		USBG->GRSTCTL = USB_GRSTCTL_TXFFLSH | (0x10 << _USB_GRSTCTL_TXFNUM_SHIFT);
		while (USBG->GRSTCTL & USB_GRSTCTL_TXFFLSH) {}

		for (int i = 0; i < 3; i++)
		{
			USBDIEP[i]->DIEPCTL  = 0;
			USBDIEP[i]->DIEPTSIZ = 0;
			USBDIEP[i]->DIEPINT  = 0xFFFFFFFF;
		}

		for (int i = 0; i < 3; i++)
		{
			USBDOEP[i]->DOEPCTL  = 0;
			USBDOEP[i]->DOEPTSIZ = 0;
			USBDOEP[i]->DOEPINT  = 0xFFFFFFFF;
		}

		USBD->DAINTMSK = USB_DAINTMSK_INEPMSK0 | USB_DAINTMSK_OUTEPMSK0 | EP1::INTMASK | EP2::INTMASK | EP3::INTMASK;
		USBD->DOEPMSK  = USB_DOEPMSK_SETUPMSK  | USB_DOEPMSK_XFERCOMPLMSK
#ifdef USB_DOEP0INT_STUPPKTRCVD
			| USB_DOEPMSK_STSPHSERCVDMSK
#endif
			;
		USBD->DIEPMSK  = USB_DIEPMSK_XFERCOMPLMSK;
		USBD->DCFG &= ~_USB_DCFG_DEVADDR_MASK;

		EP0::init();
		EP1::init();
		USBG->GINTMSK = 0;
		USBG->GINTSTS = 0xFFFFFFFF;
		USBG->GINTMSK = USB_GINTMSK_USBSUSPMSK     |
			USB_GINTMSK_USBRSTMSK      |
			USB_GINTMSK_ENUMDONEMSK    |
			USB_GINTMSK_IEPINTMSK      |
			USB_GINTMSK_OEPINTMSK      |
			USB_GINTMSK_WKUPINTMSK;
	}

	static void handle_USB_GINTSTS_USBSUSP(void) {
		USBG->GINTSTS = USB_GINTSTS_USBSUSP;
	}

	static void handle_USB_GINTSTS_WKUPINT(void) {
		USBG->GINTSTS = USB_GINTSTS_WKUPINT;
	}

	static void handle_USB_GINTSTS_IEPINT(void) {
		uint32_t ep_status = USBD->DAINT & USBD->DAINTMSK;
		if (ep_status & USB_DAINT_INEPINT0) EP0::handle_in();
		if (ep_status & USB_DAINT_INEPINT1) EP1::handle_in();
		if (ep_status & USB_DAINT_INEPINT2) EP2::handle_in();
		if (ep_status & USB_DAINT_INEPINT3) EP3::handle_in();
	}

	static void handle_USB_GINTSTS_OEPINT(void) {
		uint32_t ep_status = USBD->DAINT & USBD->DAINTMSK;
		if (ep_status & USB_DAINT_OUTEPINT0) EP0::handle_out();
		if (ep_status & USB_DAINT_OUTEPINT1) EP1::handle_out();
		if (ep_status & USB_DAINT_OUTEPINT2) EP2::handle_out();
		if (ep_status & USB_DAINT_OUTEPINT3) EP3::handle_out();
	}

	static void handle_irq(void) {
		uint32_t status;

		INT_Disable();

		status = USBG->GINTSTS;
		status &= USBG->GINTMSK;

		HANDLE_INT(USB_GINTSTS_RESETDET  );
		HANDLE_INT(USB_GINTSTS_WKUPINT   );
		HANDLE_INT(USB_GINTSTS_USBSUSP   );
		HANDLE_INT(USB_GINTSTS_SOF       );
		HANDLE_INT(USB_GINTSTS_ENUMDONE  );
		HANDLE_INT(USB_GINTSTS_USBRST    );
		HANDLE_INT(USB_GINTSTS_IEPINT    );
		HANDLE_INT(USB_GINTSTS_OEPINT    );

		INT_Enable();
	}

};

};
