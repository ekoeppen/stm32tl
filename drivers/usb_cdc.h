#pragma once

#include <usb/usb.h>
#include <string.h>
#include <peripherals.h>

namespace USB_T {

struct CDC_CORE {
	static constexpr uint8_t device_descriptor[18] = {
		0x12, 0x01, 0x10, 0x01, 0x02, 0x00, 0x00, 0x40, 0xD8, 0x04, 0x0A, 0x00, 0x00, 0x01, 0x01, 0x02,
		0x03, 0x01
	};
	static constexpr uint8_t config_descriptor[62] = {
		0x09, 0x02, 0x3E, 0x00, 0x02, 0x01, 0x00, 0xC0, 0x80,
		0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x00,
		0x05, 0x24, 0x01, 0x00, 0x01,
		0x04, 0x24, 0x02, 0x06,
		0x05, 0x24, 0x06, 0x00, 0x01,
		0x07, 0x05, 0x82, 0x03, 0x10, 0x00, 0x20,
		0x09, 0x04, 0x01, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00,
		0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x20,
		0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x20,
	};

	static constexpr uint8_t manufacturer_string_descriptor[14] = {
		0x0E, 0x03, 0x4F, 0x00, 0x6C, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x78, 0x00
	};
	static constexpr uint8_t product_string_descriptor[32] = {
		0x20, 0x03, 0x4F, 0x00, 0x6C, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x78, 0x00, 0x69, 0x00,
		0x6E, 0x00, 0x6F, 0x00, 0x2D, 0x00, 0x53, 0x00, 0x54, 0x00, 0x4D, 0x00, 0x33, 0x00, 0x32, 0x00
	};

	static constexpr uint8_t languages_descriptor[4] = {
		0x04, 0x03, 0x09, 0x04
	};

	static constexpr uint8_t line_coding[7] = {0x00, 0xc2, 0x01, 0x00, 0x00, 0x00, 0x08};

	static void get_serial_number_string_descriptor(const uint8_t **descriptor, uint16_t *length) {
		static uint8_t data[2 + 96 / 8 * 4];
		memset(data, 0, sizeof(data));
		*length = sizeof(data);
		data[0] = sizeof(data); data[1] = 3;
		for (int i = 0; i < 96 / 8; i++) {
			data[i * 4 + 2] = HEX_DIGIT(Unique_ID[i] >> 4);
			data[i * 4 + 4] = HEX_DIGIT(Unique_ID[i] & 0x0f);
		}
		*descriptor = data;
	};

	static bool get_descriptor(uint8_t type, uint8_t index, const uint8_t **descriptor, uint16_t *length)
	{
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
			default: r = false;
			}
			break;
		default: r = false;
		}
		return r;
	}

	static bool handle_request(SETUP_PACKET *packet, const uint8_t **response_data, uint16_t *length) {
		bool r = true;
		switch (packet->bRequest) {
		case 0x20:
		case 0x22: *length = 0; break;
		case 0x21: *response_data = line_coding; *length = 7; break;
		default: r = false;
		}
		return r;
	}

	typedef SIMPLE_ENDPOINT<1, BULK, 1, 64, 0, NAK, DISABLED> IN_HANDLER;
	typedef SIMPLE_ENDPOINT<2, INTERRUPT, 2, 16, 0, NAK, DISABLED> CONTROL_HANDLER;
	typedef SIMPLE_ENDPOINT<3, BULK, 3, 0, 64, DISABLED, VALID> OUT_HANDLER;

};

constexpr uint8_t CDC_CORE::device_descriptor[18];
constexpr uint8_t CDC_CORE::config_descriptor[62];
constexpr uint8_t CDC_CORE::manufacturer_string_descriptor[14];
constexpr uint8_t CDC_CORE::product_string_descriptor[32];
constexpr uint8_t CDC_CORE::languages_descriptor[4];
constexpr uint8_t CDC_CORE::line_coding[7];

struct CDC {
	typedef T<CDC_CORE::get_descriptor, CDC_CORE::handle_request, default_irq_callback, CDC_CORE::IN_HANDLER, CDC_CORE::CONTROL_HANDLER, CDC_CORE::OUT_HANDLER> USB_DRIVER;

	static constexpr bool enabled(void) { return true; }

	static void init(void) {
		USB_DRIVER::init();
	}

	static bool handle_irq(void) {
		return USB_DRIVER::handle_irq();
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void write(uint8_t *data, uint32_t count) {
		CDC_CORE::IN_HANDLER::write<TIMEOUT>(data, count);
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint32_t read(uint8_t *data, uint32_t count) {
		CDC_CORE::OUT_HANDLER::read<TIMEOUT>(data, count);
		return CDC_CORE::OUT_HANDLER::rx_current;
	}

	static bool eof(void) {
		return CDC_CORE::OUT_HANDLER::rx_current == 0;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static char getc(void) {
		char r;
		read<TIMEOUT>((uint8_t *) &r, 1);
		return r;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void putc(char c) {
		write<TIMEOUT>((uint8_t *) &c, 1);
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint32_t gets(char *s, uint32_t size) {
		uint32_t n = 0;
		while (!TIMEOUT::triggered() && n < size) {
			read<TIMEOUT>((uint8_t *) s, 1);
			s++;
			n++;
			if (*(s - 1) == '\n') break;
		}
		return n;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void puts(const char *s) {
		uint32_t n;
		const char *c;
		for (c = s, n = 0; *c; c++, n++);
		write<TIMEOUT>((uint8_t *) s, n);
	}
};

}
