#pragma once

#include <stdlib.h>
#include <usb/definitions.h>

namespace USB_T {

template<const uint8_t NUMBER,
	const uint16_t MAX_TX_COUNT,
	const uint16_t MAX_RX_COUNT>
struct ENDPOINT {
	static constexpr uint8_t number = NUMBER;
	static constexpr uint16_t max_tx_count = MAX_TX_COUNT;
	static constexpr uint16_t max_rx_count = MAX_RX_COUNT;
	static uint8_t rx_buffer[MAX_RX_COUNT] __attribute((aligned(4)));
	static uint8_t tx_buffer[MAX_TX_COUNT] __attribute((aligned(4)));

	static void start_rx(void) {
		USBDOEP[0]->DOEPDMA = (uint32_t) rx_buffer;
		USBDOEP[0]->DOEPTSIZ = ((8*3) << _USB_DOEP0TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DOEP0TSIZ_PKTCNT_SHIFT) | (3 << _USB_DOEP0TSIZ_SUPCNT_SHIFT);
		USBDOEP[0]->DOEPCTL = (USBDOEP[0]->DOEPCTL & ~DEPCTL_WO_BITMASK) | USB_DOEP0CTL_EPENA | USB_DOEP0CTL_CNAK | _USB_DOEP0CTL_MPS_64B;
	}

	static void start_tx(const uint8_t *data, uint32_t size) {
		USBDIEP[0]->DIEPDMA = (uint32_t) tx_buffer;
		for (size_t i = 0; i < size; i++) {
			tx_buffer[i] = data[i];
		}
		USBDIEP[0]->DIEPTSIZ = (size << _USB_DIEP0TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DIEP0TSIZ_PKTCNT_SHIFT);
		USBDIEP[0]->DIEPCTL = (USBDIEP[0]->DIEPCTL & ~DEPCTL_WO_BITMASK) | USB_DIEP0CTL_CNAK | USB_DIEP0CTL_EPENA | _USB_DIEP0CTL_MPS_64B;
	}

};

template<const uint8_t NUMBER, const uint16_t MAX_TX_COUNT, const uint16_t MAX_RX_COUNT>
uint8_t ENDPOINT<NUMBER, MAX_TX_COUNT, MAX_RX_COUNT>::rx_buffer[MAX_RX_COUNT];

template<const uint8_t NUMBER, const uint16_t MAX_TX_COUNT, const uint16_t MAX_RX_COUNT>
uint8_t ENDPOINT<NUMBER, MAX_TX_COUNT, MAX_RX_COUNT>::tx_buffer[MAX_TX_COUNT];

template<DESCRIPTOR_CALLBACK get_descriptor_callback,
	SETUP_CALLBACK setup_callback>
struct EP0_HANDLER {
	typedef ENDPOINT<0, 64, 64> EP;

	static void init(void) {
		EP::start_rx();
	}

	static void standard_request(SETUP_PACKET *setup, const uint8_t **data, uint32_t *size) {
		data = NULL;
		size = 0;
		switch (setup->bRequest) {
			case GET_DESCRIPTOR: get_descriptor_callback(setup->wValue >> 8, setup->wValue & 0xff, data, size);
				break;
			case SET_ADDRESS:
				USBD->DCFG = (USBD->DCFG & ~_USB_DCFG_DEVADDR_MASK) | (setup->wValue << _USB_DCFG_DEVADDR_SHIFT);
				break;
			case SET_CONFIGURATION:
			default:
				break;
		}
	}

	static void setup_handler(SETUP_PACKET *setup) {
		uint32_t size = 0;
		const uint8_t *data = 0;
		switch (setup->bmRequestType & 0x60) {
			case 0x00: standard_request(setup, &data, &size); break;
			case 0x20:
			case 0x40: setup_callback(setup, &data, &size); break;
			default: break;
		}
		if (size > setup->wLength) size = setup->wLength;
		EP::start_tx(data, size);
	}

	static void handle_in(void) {
		uint32_t status = USBDIEP[0]->DIEPINT;
		if (status & USB_DIEP_INT_XFERCOMPL) {
			USBDIEP[0]->DIEPINT = USB_DIEP_INT_XFERCOMPL;
			if (USBDIEP[0]->DIEPINT & USB_DIEP_INT_NAKINTRPT) {
				USBDIEP[0]->DIEPINT = USB_DIEP_INT_NAKINTRPT;
			}
			EP::start_rx();
		}
	}

#ifdef USB_DOEP0INT_STUPPKTRCVD
	static void handle_out(void) {
		uint32_t status = USBDOEP[0]->DOEPINT;
		SETUP_PACKET *setup;

		if (status & USB_DOEP0INT_XFERCOMPL) {
			if (status & USB_DOEP0INT_STUPPKTRCVD) {
				USBDOEP[0]->DOEPINT = USB_DOEP0INT_STUPPKTRCVD;
			}
			status = USBDOEP[0]->DOEPINT;
			USBDOEP[0]->DOEPINT = USB_DOEP0INT_XFERCOMPL;
			if (status & USB_DOEP0INT_SETUP) {
				USBDOEP[0]->DOEPINT = USB_DOEP0INT_SETUP;
				status &= ~USB_DOEP0INT_SETUP;
				int supCnt = (USBDOEP[0]->DOEPTSIZ & _USB_DOEP0TSIZ_SUPCNT_MASK) >> _USB_DOEP0TSIZ_SUPCNT_SHIFT;
				if (supCnt == 3) supCnt = 2;
				setup = &((SETUP_PACKET *) EP::rx_buffer)[2 - supCnt];
				setup_handler(setup);
			}
			EP::start_rx();
		}
		if (status & USB_DOEP0INT_STSPHSERCVD) {
			USBDOEP[0]->DOEPINT = USB_DOEP0INT_STSPHSERCVD;
		}

		if (status & USB_DOEP0INT_SETUP) {
			USBDOEP[0]->DOEPINT = USB_DOEP0INT_SETUP;
			int supCnt = (USBDOEP[0]->DOEPTSIZ & _USB_DOEP0TSIZ_SUPCNT_MASK) >> _USB_DOEP0TSIZ_SUPCNT_SHIFT;
			if (supCnt == 3) supCnt = 2;
			setup = &((SETUP_PACKET *) EP::rx_buffer)[2 - supCnt];
			setup_handler(setup);
			if (setup->wLength > 0) {
				EP::start_rx();
			}
		}

	}
#else
	static void handle_out(void) {
		uint32_t status = USBDOEP[0]->DOEPINT;
		SETUP_PACKET *setup;

		if (status & USB_DOEP0INT_XFERCOMPL) {
			status = USBDOEP[0]->DOEPINT;
			USBDOEP[0]->DOEPINT = USB_DOEP0INT_XFERCOMPL;
			if (status & USB_DOEP0INT_SETUP) {
				USBDOEP[0]->DOEPINT = USB_DOEP0INT_SETUP;
				status &= ~USB_DOEP0INT_SETUP;
				int supCnt = (USBDOEP[0]->DOEPTSIZ & _USB_DOEP0TSIZ_SUPCNT_MASK) >> _USB_DOEP0TSIZ_SUPCNT_SHIFT;
				if (supCnt == 3) supCnt = 2;
				setup = &((SETUP_PACKET *) EP::rx_buffer)[2 - supCnt];
				setup_handler(setup);
			}
			EP::start_rx();
		}

		if (status & USB_DOEP0INT_SETUP) {
			USBDOEP[0]->DOEPINT = USB_DOEP0INT_SETUP;
			int supCnt = (USBDOEP[0]->DOEPTSIZ & _USB_DOEP0TSIZ_SUPCNT_MASK) >> _USB_DOEP0TSIZ_SUPCNT_SHIFT;
			if (supCnt == 3) supCnt = 2;
			setup = &((SETUP_PACKET *) EP::rx_buffer)[2 - supCnt];
			setup_handler(setup);
			if (setup->wLength > 0) {
				EP::start_rx();
			}
		}

	}
#endif
};

template<const uint32_t NUMBER,
	const uint32_t TX_SIZE,
	const uint32_t RX_SIZE,
	typename TX_BUFFER,
	typename RX_BUFFER>
struct BUFFER_ENDPOINT {
	static uint8_t tx_buffer[TX_SIZE];
	static uint8_t rx_buffer[RX_SIZE];
	volatile static uint32_t tx_size;
	volatile static uint32_t rx_size;
	static constexpr uint32_t NUM = NUMBER - 1;
	static constexpr uint32_t INTMASK = (TX_SIZE > 0 ? 1 << NUMBER : 0) | (RX_SIZE > 0 ? 1 << (NUMBER + 16) : 0);

	static void init(void) {
		USBDOEP[NUM]->DOEPDMA = (uint32_t) rx_buffer;
		USBDOEP[NUM]->DOEPTSIZ = (RX_SIZE << _USB_DOEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DOEP_TSIZ_PKTCNT_SHIFT);
		USBDOEP[NUM]->DOEPCTL = USB_DOEP_CTL_EPENA | USB_DOEP_CTL_CNAK | USB_DOEP_CTL_USBACTEP | USB_DOEP_CTL_EPTYPE_INT | 64;

		USBDIEP[NUM]->DIEPDMA = (uint32_t) tx_buffer;
		USBDIEP[NUM]->DIEPTSIZ = (0 << _USB_DOEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DIEP_TSIZ_PKTCNT_SHIFT);
		USBDIEP[NUM]->DIEPCTL = USB_DIEP_CTL_SNAK | (NUMBER << _USB_DIEP_CTL_TXFNUM_SHIFT) | USB_DIEP_CTL_USBACTEP | USB_DIEP_CTL_SETD0PIDEF | USB_DIEP_CTL_EPTYPE_INT | 64;

		tx_size = 0;
		rx_size = 0;
	}

	static void update_tx_buffer(bool reuse = false) {
		size_t i;
		USBDIEP[NUM]->DIEPDMA = (uint32_t) tx_buffer;
		if (!reuse) {
			tx_size = 0;
			for (i = 0; i < TX_SIZE && !TX_BUFFER::is_empty(); i++) {
				tx_buffer[i] = TX_BUFFER::read();
			}
			tx_size = i;
		}
		USBDIEP[NUM]->DIEPTSIZ = (tx_size << _USB_DIEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DIEP_TSIZ_PKTCNT_SHIFT);
		USBDIEP[NUM]->DIEPCTL = (USBDIEP[NUM]->DIEPCTL & ~DEPCTL_WO_BITMASK) | USB_DIEP_CTL_CNAK | USB_DIEP_CTL_EPENA;
	}

	static void handle_in(void) {
		if (USBDIEP[NUM]->DIEPINT & USB_DIEP_INT_NAKINTRPT) {
			USBDIEP[NUM]->DIEPINT = USB_DIEP_INT_NAKINTRPT;
		}
		if (USBDIEP[NUM]->DIEPINT & USB_DIEP_INT_XFERCOMPL) {
			USBDIEP[NUM]->DIEPINT = USB_DIEP_INT_XFERCOMPL;
			tx_size = 0;
		}
		if (!TX_BUFFER::is_empty() > 0) {
			update_tx_buffer();
		} else {
			USBDIEP[NUM]->DIEPCTL = (USBDIEP[NUM]->DIEPCTL & ~DEPCTL_WO_BITMASK) | USB_DIEP_CTL_SNAK;
			tx_size = 0;
		}
	}

	static void handle_out(void) {
		uint32_t n;
		if (USBDOEP[NUM]->DOEPINT & USB_DOEP_INT_XFERCOMPL) {
			USBDOEP[NUM]->DOEPINT = USB_DOEP_INT_XFERCOMPL;
		}
		n = RX_SIZE - (USBDOEP[NUM]->DOEPTSIZ & 0x000000ff);
		for (size_t i = 0; i < n; i++) {
			RX_BUFFER::write(rx_buffer[i]);

		}
		USBDOEP[NUM]->DOEPDMA = (uint32_t) rx_buffer;
		USBDOEP[NUM]->DOEPTSIZ = (RX_SIZE << _USB_DOEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DOEP_TSIZ_PKTCNT_SHIFT);
		USBDOEP[NUM]->DOEPCTL = USB_DOEP_CTL_EPENA | USB_DOEP_CTL_CNAK | USB_DOEP_CTL_USBACTEP | USB_DOEP_CTL_EPTYPE_INT | RX_SIZE;
		exit_idle();
	}

	static void start_tx(void) {
#if 0
		while (tx_size > 0) {
			__WFI();
		}
		update_tx_buffer();
#else
		if (tx_size == 0) {
			update_tx_buffer();
		}
#endif
	}
};

template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE, typename TX_BUFFER, typename RX_BUFFER>
uint8_t BUFFER_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE, TX_BUFFER, RX_BUFFER>::tx_buffer[TX_SIZE] __attribute__((aligned(4)));
template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE, typename TX_BUFFER, typename RX_BUFFER>
uint8_t BUFFER_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE, TX_BUFFER, RX_BUFFER>::rx_buffer[RX_SIZE] __attribute__((aligned(4)));
template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE, typename TX_BUFFER, typename RX_BUFFER>
volatile uint32_t BUFFER_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE, TX_BUFFER, RX_BUFFER>::tx_size;
template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE, typename TX_BUFFER, typename RX_BUFFER>
volatile uint32_t BUFFER_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE, TX_BUFFER, RX_BUFFER>::rx_size;

template<const uint32_t NUMBER,
	const uint32_t TX_SIZE,
	const uint32_t RX_SIZE>
struct SIMPLE_ENDPOINT {
	static uint8_t tx_buffer[TX_SIZE];
	static uint8_t rx_buffer[RX_SIZE];
	volatile static uint32_t tx_size;
	volatile static uint32_t rx_size;
	static constexpr uint32_t NUM = NUMBER - 1;
	static constexpr uint32_t INTMASK = (TX_SIZE > 0 ? 1 << NUMBER : 0) | (RX_SIZE > 0 ? 1 << (NUMBER + 16) : 0);

	static void init(void) {
		USBDOEP[NUM]->DOEPDMA = (uint32_t) rx_buffer;
		USBDOEP[NUM]->DOEPTSIZ = (RX_SIZE << _USB_DOEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DOEP_TSIZ_PKTCNT_SHIFT);
		USBDOEP[NUM]->DOEPCTL = USB_DOEP_CTL_EPENA | USB_DOEP_CTL_CNAK | USB_DOEP_CTL_USBACTEP | USB_DOEP_CTL_EPTYPE_INT | 64;

		USBDIEP[NUM]->DIEPDMA = (uint32_t) tx_buffer;
		USBDIEP[NUM]->DIEPTSIZ = (0 << _USB_DOEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DIEP_TSIZ_PKTCNT_SHIFT);
		USBDIEP[NUM]->DIEPCTL = USB_DIEP_CTL_SNAK | (NUMBER << _USB_DIEP_CTL_TXFNUM_SHIFT) | USB_DIEP_CTL_USBACTEP | USB_DIEP_CTL_SETD0PIDEF | USB_DIEP_CTL_EPTYPE_INT | 64;

		tx_size = 0;
		rx_size = 0;
	}

	static void handle_in(void) {
		if (USBDIEP[NUM]->DIEPINT & USB_DIEP_INT_NAKINTRPT) {
			USBDIEP[NUM]->DIEPINT = USB_DIEP_INT_NAKINTRPT;
		}
		if (USBDIEP[NUM]->DIEPINT & USB_DIEP_INT_XFERCOMPL) {
			USBDIEP[NUM]->DIEPINT = USB_DIEP_INT_XFERCOMPL;
		}
		USBDIEP[NUM]->DIEPCTL = (USBDIEP[NUM]->DIEPCTL & ~DEPCTL_WO_BITMASK) | USB_DIEP_CTL_SNAK;
		tx_size = 0;
	}

	static void handle_out(void) {
		uint32_t n;
		if (USBDOEP[NUM]->DOEPINT & USB_DOEP_INT_XFERCOMPL) {
			USBDOEP[NUM]->DOEPINT = USB_DOEP_INT_XFERCOMPL;
		}
		n = RX_SIZE - (USBDOEP[NUM]->DOEPTSIZ & 0x000000ff);
		USBDOEP[NUM]->DOEPDMA = (uint32_t) rx_buffer;
		USBDOEP[NUM]->DOEPTSIZ = (RX_SIZE << _USB_DOEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DOEP_TSIZ_PKTCNT_SHIFT);
		USBDOEP[NUM]->DOEPCTL = USB_DOEP_CTL_EPENA | USB_DOEP_CTL_CNAK | USB_DOEP_CTL_USBACTEP | USB_DOEP_CTL_EPTYPE_INT | RX_SIZE;
		exit_idle();
	}

	static void start_tx(void) {
		USBDIEP[NUM]->DIEPTSIZ = (tx_size << _USB_DIEP_TSIZ_XFERSIZE_SHIFT) | (1 << _USB_DIEP_TSIZ_PKTCNT_SHIFT);
		USBDIEP[NUM]->DIEPCTL = (USBDIEP[NUM]->DIEPCTL & ~DEPCTL_WO_BITMASK) | USB_DIEP_CTL_CNAK | USB_DIEP_CTL_EPENA;
	}
};

template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE>
uint8_t SIMPLE_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE>::tx_buffer[TX_SIZE] __attribute__((aligned(4)));
template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE>
uint8_t SIMPLE_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE>::rx_buffer[RX_SIZE] __attribute__((aligned(4)));
template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE>
volatile uint32_t SIMPLE_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE>::tx_size;
template<const uint32_t NUMBER, const uint32_t TX_SIZE, const uint32_t RX_SIZE>
volatile uint32_t SIMPLE_ENDPOINT<NUMBER, TX_SIZE, RX_SIZE>::rx_size;

};
