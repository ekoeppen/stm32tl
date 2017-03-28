#pragma once

#include <io.h>
#include <string.h>

namespace USB_T {

template<typename TX_BUFFER,
	typename RX_BUFFER>
struct CP2102 {
	volatile static bool initialized;
	volatile static uint8_t handshaking_state;

	static constexpr uint8_t device_descriptor[18] = {
		0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x40, 0xc4, 0x10, 0x60, 0xea, 0x00, 0x01, 0x01, 0x02,
		0x03, 0x01
	};
	static constexpr uint8_t config_descriptor[32] = {
		0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
		0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x00, 0x00, 0x02,
		0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00,
		0x07, 0x05, 0x01, 0x02, 0x40, 0x00, 0x00
	};

	static constexpr uint8_t manufacturer_string_descriptor[10] = {
		0x0a, 0x03,
		'4', 0x00, '0', 0x00, 'H', 0x00, 'z', 0x00
	};

	static constexpr uint8_t product_string_descriptor[34] = {
		0x22, 0x03,
		'S', 0x00, 'T', 0x00, 'M', 0x00, '3', 0x00, '2', 0x00, ' ', 0x00, 'U', 0x00, 'S', 0x00,
		'B', 0x00, ' ', 0x00, 'B', 0x00, 'r', 0x00, 'i', 0x00, 'd', 0x00, 'g', 0x00, 'e', 0x00,
	};

	static constexpr uint8_t languages_descriptor[4] = {
		0x04, 0x03, 0x09, 0x04
	};

#ifdef STM32F0xx
	static constexpr uint8_t *u_id = (uint8_t *) 0x1ffff7ac;
#else
	static constexpr uint8_t *u_id = (uint8_t *) 0x1ffff7e8;
#endif

	static void get_serial_number_string_descriptor(const uint8_t **descriptor, uint16_t *length) {
		static uint8_t data[2 + 96 / 8 * 4];
		memset(data, 0, sizeof(data));
		*length = sizeof(data);
		data[0] = sizeof(data); data[1] = 3;
		for (int i = 0; i < 96 / 8; i++) {
			data[i * 4 + 2] = HEX_DIGIT(u_id[i] >> 4);
			data[i * 4 + 4] = HEX_DIGIT(u_id[i] & 0x0f);
		}
		*descriptor = data;
	};

	static bool get_descriptor(uint8_t type, uint8_t index, const uint8_t **descriptor, uint16_t *length) {
		bool r = true;
		switch (type) {
		case DEVICE_DESCRIPTOR: *descriptor = device_descriptor; *length = sizeof(device_descriptor); break;
		case CONFIG_DESCRIPTOR: *descriptor = config_descriptor; *length = sizeof(config_descriptor); break;
		case STRING_DESCRIPTOR:
			switch (index) {
			case 0: *descriptor = languages_descriptor; *length = sizeof(languages_descriptor); break;
			case 1: *descriptor = manufacturer_string_descriptor; *length = sizeof(manufacturer_string_descriptor); break;
			case 2: *descriptor = product_string_descriptor; *length = sizeof(product_string_descriptor); break;
			case 3: get_serial_number_string_descriptor(descriptor, length); break;
			default: r = false; break;
			}
			break;
		default: r = false;
		}
		return r;
	}

	static bool handle_request(SETUP_PACKET *packet, const uint8_t **response_data, uint16_t *length) {
		bool r = true;
		if (packet->bmRequestType == 0x80) {
			*response_data = (const uint8_t *) "\x00\x00"; *length = 2;
		} else {
			switch (packet->bRequest) {
			case 0x00: if (packet->wValue) initialized = true; *length = 0; break;
			case 0x04: *response_data = (const uint8_t *) "\x08\x00"; *length = 2; break;
			case 0x07: handshaking_state = (handshaking_state & ~(packet->wValue >> 8)) | (packet->wValue & 0x3); *length = 0; break;
			case 0x08: *response_data = (const uint8_t *) &handshaking_state; *length = 1; break;
			case 0x10: *response_data = (const uint8_t *) "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"; *length = 19; break;
			case 0x1d: *response_data = (const uint8_t *) "\x00\xc2\x01\x00"; *length = 4; break;
			case 0x11: NVIC_SystemReset(); break;
			case 0xff: *response_data = (const uint8_t *) "\x02"; *length = 1; break;
			case 0x0f: *response_data = (const uint8_t *) "\x42\x00\x00\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00\xC0\xC6\x2D\x00\x01\x00\x00\x00\x3F\x01\x00\x00\x7F\x00\x00\x00\xF0\xFF\x07\x10\x0F\x00\x07\x1F\x00\x02\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x33\x00\x2E\x00"; *length = 64; break;
			case 0x03:
			case 0x05:
			case 0x13:
			case 0x19:
			case 0x1e: *length = 0; break;
			default: r = false;
			};
		}
		return r;
	}

	typedef BUFFER_ENDPOINT<1, BULK, 1, 64, 64, NAK, VALID, TX_BUFFER, RX_BUFFER> DATA_ENDPOINT;
	typedef T<get_descriptor, handle_request, default_irq_callback, DATA_ENDPOINT> USB_DRIVER;

	static void init(void) {
		USB_DRIVER::init();
		handshaking_state = 0x30;
	}

	static bool enabled(void) {
		return true;
	}

	static bool handle_irq(void) {
		return USB_DRIVER::handle_irq();
	}

	static void tx_start(void) {
		if (initialized) {
			DATA_ENDPOINT::tx_start();
		}
	}

	static bool eof(void) {
		return RX_BUFFER::is_empty();
	}

	static bool dtr(void) {
		return handshaking_state & 0x01;
	}

	static bool rts(void) {
		return handshaking_state & 0x02;
	}

	static void set_cts(bool b) {
		handshaking_state = handshaking_state & ~(0x10) | (b ? 0x10 : 0);
	}

	static void set_dsr(bool b) {
		handshaking_state = handshaking_state & ~(0x20) | (b ? 0x20 : 0);
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void putc(char data) {
		TX_BUFFER::write(data);
		tx_start();
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void puts(const char *data) {
		while (*data) {
			TX_BUFFER::write(*data);
			data++;
		}
		tx_start();
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static char getc() {
		while (!TIMEOUT::triggered() && eof()) {
			enter_idle();
		}
		if (!eof()) {
			return RX_BUFFER::read();
		}
		return 0;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static char *gets(char *buffer, int size) {
		static int n = 0;
		char c = -1;
		char *r = 0;
		bool eol = false;
		do {
			while (!TIMEOUT::triggered() && eof()) {
				enter_idle();
			}
			if (!eof()) {
				c = RX_BUFFER::read();
				if (n < size - 1 && c != '\n' && c != '\r') {
					putc(c);
					buffer[n++] = c;
				} else {
					eol = true;
				}
			}
		} while (!TIMEOUT::triggered() && !eol);
		if (eol) {
			buffer[n] = 0;
			n = 0;
			r = buffer;
		}
		return r;
	}

};

template<typename TX_BUFFER, typename RX_BUFFER>
volatile bool CP2102<TX_BUFFER, RX_BUFFER>::initialized;

template<typename TX_BUFFER, typename RX_BUFFER>
volatile uint8_t CP2102<TX_BUFFER, RX_BUFFER>::handshaking_state;

template<typename TX_BUFFER, typename RX_BUFFER>
constexpr uint8_t CP2102<TX_BUFFER, RX_BUFFER>::device_descriptor[18];

template<typename TX_BUFFER, typename RX_BUFFER>
constexpr uint8_t CP2102<TX_BUFFER, RX_BUFFER>::config_descriptor[32];

template<typename TX_BUFFER, typename RX_BUFFER>
constexpr uint8_t CP2102<TX_BUFFER, RX_BUFFER>::manufacturer_string_descriptor[10];

template<typename TX_BUFFER, typename RX_BUFFER>
constexpr uint8_t CP2102<TX_BUFFER, RX_BUFFER>::product_string_descriptor[34];

template<typename TX_BUFFER, typename RX_BUFFER>
constexpr uint8_t CP2102<TX_BUFFER, RX_BUFFER>::languages_descriptor[4];

}
