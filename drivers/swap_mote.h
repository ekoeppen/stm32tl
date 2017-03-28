#pragma once

#define SWAP_CONFIG_DATA 1

enum PACKET_TYPE {
	STATUS = 0,
	QUERY = 1,
	COMMAND = 2
};

enum REGISTER_NUMBER {
	PRODUCT_CODE_REG = 0,
	HW_VERSION_REG = 1,
	FW_VERSION_REG = 2,
	SYSTEM_STATE_REG = 3,
	CHANNEL_REG = 4,
	SECURITY_REG = 5,
	PASSWORD_REG = 6,
	NONCE_REG = 7,
	NETWORK_ID_REG = 8,
	DEVICE_ADDR_REG = 9,
	TX_INTERVAL_REG = 10,
	UNUSED_REG = 255
};

enum SYSTEM_STATE {
	RESTART = 0,
	RXON = 1,
	RXOFF = 2,
	SYNC = 3,
	LOWBAT = 4
};

struct SWAP_PACKET {
	uint8_t dest;
	uint8_t src;
	uint8_t hop_secu;
	uint8_t nonce;
	uint8_t function;
	uint8_t reg_addr;
	uint8_t reg_id;
	uint8_t reg_value[16];
	uint8_t len;
};

struct CONFIGURATION_DATA {
	uint8_t tag;
	uint8_t length;
	uint8_t address[5];
	uint8_t channel;
	uint16_t tx_interval;

	void update(uint8_t ch, uint8_t addr, uint16_t tx_int) {
		tag = SWAP_CONFIG_DATA;
		length = sizeof(CONFIGURATION_DATA) - 2;
		address[1] = address[2] = address[3] = address[4] = 0xf0;
		address[0] = addr;
		channel = ch;
		tx_interval = tx_int;
	}
};

struct CONFIG_STORAGE_UNUSED {
	static void write(const void *data, unsigned size) { }
};

struct UNUSED_REGISTER {
	static bool write(SWAP_PACKET& packet) { return false; }
	static bool handle_command(SWAP_PACKET& packet) { return false; }
	static bool handle_query(SWAP_PACKET& packet) { return false; }
	static void update(void) { }
};

template<const uint32_t ID>
struct GENERIC_REGISTER_T {
	static constexpr uint8_t reg_id = ID;
	static bool write(SWAP_PACKET& packet, uint8_t *data, uint8_t len) {
		packet.reg_id = ID;
		for (int i = 0; i < len; i++) {
			packet.reg_value[i] = data[i];
		}
		packet.len = 7 + len;
		return true;
	};
	static bool handle_command(SWAP_PACKET& packet) { return false; }
	static bool handle_query(SWAP_PACKET& packet) { return false; }
	static void update(void) { };
};

template<const uint32_t MANUFACTURER_ID, const uint32_t PRODUCT_ID>
struct PRODUCT_CODE_REGISTER_T {
	static bool write(SWAP_PACKET& packet) {
		packet.reg_id = PRODUCT_CODE_REG;
		packet.reg_value[0] = (MANUFACTURER_ID >> 24) & 0xff; packet.reg_value[1] = (MANUFACTURER_ID >> 16) & 0xff;
		packet.reg_value[2] = (MANUFACTURER_ID >>  8) & 0xff; packet.reg_value[3] =  MANUFACTURER_ID        & 0xff;
		packet.reg_value[4] = (PRODUCT_ID >> 24) & 0xff; packet.reg_value[5] = (PRODUCT_ID >> 16) & 0xff;
		packet.reg_value[6] = (PRODUCT_ID >>  8) & 0xff; packet.reg_value[7] =  PRODUCT_ID        & 0xff;
		packet.len = 7 + 8;
		return true;
	};
	static bool handle_command(SWAP_PACKET& packet) { return false; }
	static bool handle_query(SWAP_PACKET& packet) { return false; }
	static void update(void) { };
};

template<uint8_t *STATE>
struct SYSTEM_STATE_REGISTER_T {
	static bool write(SWAP_PACKET& packet) {
		packet.reg_id = SYSTEM_STATE_REG;
		packet.reg_value[0] = *STATE;
		packet.len = 7 + 1;
		return true;
	};
	static bool handle_command(SWAP_PACKET& packet) { return false; }
	static bool handle_query(SWAP_PACKET& packet) { return false; }
	static void update(void) { };
};

template<CONFIGURATION_DATA *config, typename CONFIG_STORAGE>
struct CHANNEL_REGISTER_T {
	static bool write(SWAP_PACKET& packet) {
		packet.reg_id = CHANNEL_REG;
		packet.reg_value[0] = config->channel;
		packet.len = 7 + 1;
		return true;
	};
	static bool handle_command(SWAP_PACKET& packet) {
		bool r = false;
		if (packet.reg_id == CHANNEL_REG) {
			CONFIGURATION_DATA c;
			c.update(packet.reg_value[0], config->address[0], config->tx_interval);
			CONFIG_STORAGE::write(&c, sizeof(c));
			r = true;
		} return r;
	}
	static bool handle_query(SWAP_PACKET& packet) {
		return false;
	}
	static void update(void) { };
};

template<CONFIGURATION_DATA *config, typename CONFIG_STORAGE>
struct DEVICE_ADDR_REGISTER_T {
	static bool write(SWAP_PACKET& packet) {
		packet.reg_id = DEVICE_ADDR_REG;
		packet.reg_value[0] = config->address[0];
		packet.len = 7 + 1;
		return true;
	};
	static bool handle_command(SWAP_PACKET& packet) {
		bool r = false;
		if (packet.reg_id == DEVICE_ADDR_REG) {
			CONFIGURATION_DATA c;
			c.update(config->channel, packet.reg_value[0], config->tx_interval);
			CONFIG_STORAGE::write(&c, sizeof(c));
			r = true;
		}
		return r;
	}
	static bool handle_query(SWAP_PACKET& packet) {
		return false;
	}
	static void update(void) { };
};

template<CONFIGURATION_DATA *config, typename CONFIG_STORAGE>
struct TX_INTERVAL_REGISTER_T {
	static bool write(SWAP_PACKET& packet) {
		packet.reg_id = TX_INTERVAL_REG;
		packet.reg_value[0] = config->tx_interval >> 8;
		packet.reg_value[1] = config->tx_interval & 0xff;
		packet.len = 7 + 2;
		return true;
	};
	static bool handle_command(SWAP_PACKET& packet) {
		bool r = false;
		if (packet.reg_id == TX_INTERVAL_REG) {
			CONFIGURATION_DATA c;
			c.update(config->channel, config->address[0], (packet.reg_value[0] << 8) + packet.reg_value[1]);
			CONFIG_STORAGE::write(&c, sizeof(c));
			r = true;
		}
		return r;
	}
	static bool handle_query(SWAP_PACKET& packet) {
		return false;
	}
	static void update(void) { };
};

typedef bool(*STATUS_CALLBACK)(void);

template<const uint32_t MANUFACTURER_ID,
	const uint32_t PRODUCT_ID,
	const uint32_t HARDWARE_VERSION,
	const uint32_t FIRMWARE_VERSION,
	typename RADIO,
	const uint8_t DEFAULT_CHANNEL,
	typename CONFIG_STORAGE = CONFIG_STORAGE_UNUSED,
	typename REGISTER1 = UNUSED_REGISTER,
	typename REGISTER2 = UNUSED_REGISTER,
	typename REGISTER3 = UNUSED_REGISTER,
	typename REGISTER4 = UNUSED_REGISTER,
	typename REGISTER5 = UNUSED_REGISTER,
	typename REGISTER6 = UNUSED_REGISTER,
	typename REGISTER7 = UNUSED_REGISTER,
	typename REGISTER8 = UNUSED_REGISTER>
struct SWAP_MOTE_T {
	static SWAP_PACKET rx_packet;
	static SWAP_PACKET tx_packet;
	static uint8_t state;
	static CONFIGURATION_DATA config;
	static const uint8_t BROADCAST_ADDR[5];

	typedef PRODUCT_CODE_REGISTER_T<MANUFACTURER_ID, PRODUCT_ID> PRODUCT_CODE_REGISTER;
	typedef SYSTEM_STATE_REGISTER_T<&state> SYSTEM_STATE_REGISTER;
	typedef CHANNEL_REGISTER_T<&config, CONFIG_STORAGE> CHANNEL_REGISTER;
	typedef DEVICE_ADDR_REGISTER_T<&config, CONFIG_STORAGE> DEVICE_ADDR_REGISTER;
	typedef TX_INTERVAL_REGISTER_T<&config, CONFIG_STORAGE> TX_INTERVAL_REGISTER;

	static void start_tx(void) { RADIO::start_tx(); }
	static void start_rx(void) { RADIO::start_rx(); }

	static bool address_match(void) {
		return rx_packet.reg_addr == 0 || rx_packet.reg_addr == config.address[0];
	};

	static void send_tx_packet(uint8_t function, uint8_t dest, uint8_t reg_addr) {
		tx_packet.src = config.address[0];
		tx_packet.nonce = 0;
		tx_packet.hop_secu = 0;
		tx_packet.dest = dest;
		tx_packet.function = function;
		tx_packet.reg_addr = reg_addr;
		RADIO::tx_buffer((uint8_t *) &tx_packet, tx_packet.len, false);
	}

	static void send_status_packet(void) {
		send_tx_packet(STATUS, 0, config.address[0]);
	}

	static void send_query_packet(uint8_t dest, uint8_t reg_addr) {
		send_tx_packet(QUERY, dest, reg_addr);
	}

	static void send_command_packet(uint8_t dest, uint8_t reg_addr) {
		send_tx_packet(COMMAND, dest, reg_addr);
	}

	static void init(void) {
		RADIO::set_channel(config.channel);
		RADIO::set_rx_addr(config.address);
		RADIO::set_tx_addr(BROADCAST_ADDR);
	}

	static bool handle_status(void) {
		return false;
	}

	template<typename RX_TIMEOUT = TIMEOUT_NEVER, STATUS_CALLBACK handle_status_callback = SWAP_MOTE_T::handle_status>
	static void handle_radio(void) {
		bool send_status = false;
		uint8_t pipe;

		RADIO::start_rx();
		rx_packet.len = RADIO::template rx_buffer<RX_TIMEOUT>((uint8_t *) &rx_packet, sizeof(rx_packet) - sizeof(rx_packet.len), &pipe);
		if (rx_packet.len > 0) {
			switch (rx_packet.function) {
				case STATUS: send_status = handle_status_callback(); break;
				case QUERY: send_status = handle_query(); break;
				case COMMAND: send_status = handle_command(); break;
			}
			if (send_status) {
				RADIO::start_tx();
				send_status_packet();
			}
		}
	}

	template<typename RX_TIMEOUT>
	static void sync(void) {
		RADIO::start_tx();
		state = SYNC;
		SYSTEM_STATE_REGISTER::write(tx_packet); send_status_packet();
		while (!RX_TIMEOUT::triggered()) {
			handle_radio<RX_TIMEOUT>();
		}
		RX_TIMEOUT::disable();
	}

	static void rx_off(void) {
		state = RXOFF;
		RADIO::start_tx();
		SYSTEM_STATE_REGISTER::write(tx_packet); send_status_packet();
		RADIO::power_down();
	};

	template<typename RX_TIMEOUT>
	static void announce(void) {
		RADIO::start_tx();
		PRODUCT_CODE_REGISTER::write(tx_packet); send_status_packet();
		sync<RX_TIMEOUT>();
		rx_off();
	};

	static bool handle_query(void) {
		bool r = false;
		if (address_match()) {
			if ((SYSTEM_STATE_REGISTER::handle_query(rx_packet) && SYSTEM_STATE_REGISTER::write(tx_packet)) ||
				(CHANNEL_REGISTER::handle_query(rx_packet) && CHANNEL_REGISTER::write(tx_packet)) ||
				(DEVICE_ADDR_REGISTER::handle_query(rx_packet) && DEVICE_ADDR_REGISTER::write(tx_packet)) ||
				(TX_INTERVAL_REGISTER::handle_query(rx_packet) && TX_INTERVAL_REGISTER::write(tx_packet)) ||
				(REGISTER1::handle_query(rx_packet) && REGISTER1::write(tx_packet)) ||
				(REGISTER2::handle_query(rx_packet) && REGISTER2::write(tx_packet)) ||
				(REGISTER3::handle_query(rx_packet) && REGISTER3::write(tx_packet)) ||
				(REGISTER4::handle_query(rx_packet) && REGISTER4::write(tx_packet)) ||
				(REGISTER5::handle_query(rx_packet) && REGISTER5::write(tx_packet)) ||
				(REGISTER6::handle_query(rx_packet) && REGISTER6::write(tx_packet)) ||
				(REGISTER7::handle_query(rx_packet) && REGISTER7::write(tx_packet)) ||
				(REGISTER8::handle_query(rx_packet) && REGISTER8::write(tx_packet))) {
				r = true;
			}
		}
		return r;
	};

	static bool handle_command(void) {
		bool r = false;
		if (address_match()) {
			if ((SYSTEM_STATE_REGISTER::handle_command(rx_packet) && SYSTEM_STATE_REGISTER::write(tx_packet)) ||
				(CHANNEL_REGISTER::handle_command(rx_packet) && CHANNEL_REGISTER::write(tx_packet)) ||
				(DEVICE_ADDR_REGISTER::handle_command(rx_packet) && DEVICE_ADDR_REGISTER::write(tx_packet)) ||
				(TX_INTERVAL_REGISTER::handle_command(rx_packet) && TX_INTERVAL_REGISTER::write(tx_packet)) ||
				(REGISTER1::handle_command(rx_packet) && REGISTER1::write(tx_packet)) ||
				(REGISTER2::handle_command(rx_packet) && REGISTER2::write(tx_packet)) ||
				(REGISTER3::handle_command(rx_packet) && REGISTER3::write(tx_packet)) ||
				(REGISTER4::handle_command(rx_packet) && REGISTER4::write(tx_packet)) ||
				(REGISTER5::handle_command(rx_packet) && REGISTER5::write(tx_packet)) ||
				(REGISTER6::handle_command(rx_packet) && REGISTER6::write(tx_packet)) ||
				(REGISTER7::handle_command(rx_packet) && REGISTER7::write(tx_packet)) ||
				(REGISTER8::handle_command(rx_packet) && REGISTER8::write(tx_packet))) {
				r = true;
			}
		}
		return r;
	};

	static void update_registers(void) {
		REGISTER1::update();
		REGISTER2::update();
		REGISTER3::update();
		REGISTER4::update();
		REGISTER5::update();
		REGISTER6::update();
		REGISTER7::update();
		REGISTER8::update();
	};

	static void transmit_data(void) {
		RADIO::start_tx();
		if (REGISTER1::write(tx_packet)) send_status_packet();
		if (REGISTER2::write(tx_packet)) send_status_packet();
		if (REGISTER3::write(tx_packet)) send_status_packet();
		if (REGISTER4::write(tx_packet)) send_status_packet();
		if (REGISTER5::write(tx_packet)) send_status_packet();
		if (REGISTER6::write(tx_packet)) send_status_packet();
		if (REGISTER7::write(tx_packet)) send_status_packet();
		if (REGISTER8::write(tx_packet)) send_status_packet();
		RADIO::power_down();
	};

	template<typename SLEEP_TIMEOUT>
	static void sleep(void) {
		SLEEP_TIMEOUT::set_and_wait((uint32_t) config.tx_interval * (uint32_t) 1000);
	};

	template<typename SLEEP_TIMEOUT>
	static void run(void) {
		while (1) {
			update_registers();
			transmit_data();
			sleep<SLEEP_TIMEOUT>();
		}
	}
};

template<const uint32_t MANUFACTURER_ID, const uint32_t PRODUCT_ID, const uint32_t HARDWARE_VERSION, const uint32_t FIRMWARE_VERSION,
	typename RADIO, const uint8_t DEFAULT_CHANNEL,
	typename CONFIG_STORAGE,
	typename REGISTER1, typename REGISTER2, typename REGISTER3, typename REGISTER4,
	typename REGISTER5, typename REGISTER6, typename REGISTER7, typename REGISTER8>
SWAP_PACKET SWAP_MOTE_T<MANUFACTURER_ID, PRODUCT_ID, HARDWARE_VERSION, FIRMWARE_VERSION, RADIO, DEFAULT_CHANNEL, CONFIG_STORAGE,
	    REGISTER1, REGISTER2, REGISTER3, REGISTER4, REGISTER5, REGISTER6, REGISTER7, REGISTER8>::tx_packet;

template<const uint32_t MANUFACTURER_ID, const uint32_t PRODUCT_ID, const uint32_t HARDWARE_VERSION, const uint32_t FIRMWARE_VERSION,
	typename RADIO, const uint8_t DEFAULT_CHANNEL,
	typename CONFIG_STORAGE,
	typename REGISTER1, typename REGISTER2, typename REGISTER3, typename REGISTER4,
	typename REGISTER5, typename REGISTER6, typename REGISTER7, typename REGISTER8>
SWAP_PACKET SWAP_MOTE_T<MANUFACTURER_ID, PRODUCT_ID, HARDWARE_VERSION, FIRMWARE_VERSION, RADIO, DEFAULT_CHANNEL, CONFIG_STORAGE,
	    REGISTER1, REGISTER2, REGISTER3, REGISTER4, REGISTER5, REGISTER6, REGISTER7, REGISTER8>::rx_packet;

template<const uint32_t MANUFACTURER_ID, const uint32_t PRODUCT_ID, const uint32_t HARDWARE_VERSION, const uint32_t FIRMWARE_VERSION,
	typename RADIO, const uint8_t DEFAULT_CHANNEL,
	typename CONFIG_STORAGE,
	typename REGISTER1, typename REGISTER2, typename REGISTER3, typename REGISTER4,
	typename REGISTER5, typename REGISTER6, typename REGISTER7, typename REGISTER8>
CONFIGURATION_DATA SWAP_MOTE_T<MANUFACTURER_ID, PRODUCT_ID, HARDWARE_VERSION, FIRMWARE_VERSION, RADIO, DEFAULT_CHANNEL, CONFIG_STORAGE,
	    REGISTER1, REGISTER2, REGISTER3, REGISTER4,
	    REGISTER5, REGISTER6, REGISTER7, REGISTER8>::config /* __attribute__((section(".infod"))) */ = {
	SWAP_CONFIG_DATA,
	sizeof(CONFIGURATION_DATA) - 2,
	0x00, 0xf0, 0xf0, 0xf0, 0xf0,
	DEFAULT_CHANNEL,
	15
};

template<const uint32_t MANUFACTURER_ID, const uint32_t PRODUCT_ID, const uint32_t HARDWARE_VERSION, const uint32_t FIRMWARE_VERSION,
	typename RADIO, const uint8_t DEFAULT_CHANNEL,
	typename CONFIG_STORAGE,
	typename REGISTER1, typename REGISTER2, typename REGISTER3, typename REGISTER4,
	typename REGISTER5, typename REGISTER6, typename REGISTER7, typename REGISTER8>
const uint8_t SWAP_MOTE_T<MANUFACTURER_ID, PRODUCT_ID, HARDWARE_VERSION, FIRMWARE_VERSION, RADIO, DEFAULT_CHANNEL, CONFIG_STORAGE,
	    REGISTER1, REGISTER2, REGISTER3, REGISTER4,
	    REGISTER5, REGISTER6, REGISTER7, REGISTER8>::BROADCAST_ADDR[5] = {0x00, 0xf0, 0xf0, 0xf0, 0xf0};

template<const uint32_t MANUFACTURER_ID, const uint32_t PRODUCT_ID, const uint32_t HARDWARE_VERSION, const uint32_t FIRMWARE_VERSION,
	typename RADIO, const uint8_t DEFAULT_CHANNEL,
	typename CONFIG_STORAGE,
	typename REGISTER1, typename REGISTER2, typename REGISTER3, typename REGISTER4,
	typename REGISTER5, typename REGISTER6, typename REGISTER7, typename REGISTER8>
uint8_t SWAP_MOTE_T<MANUFACTURER_ID, PRODUCT_ID, HARDWARE_VERSION, FIRMWARE_VERSION, RADIO, DEFAULT_CHANNEL, CONFIG_STORAGE,
	    REGISTER1, REGISTER2, REGISTER3, REGISTER4, REGISTER5, REGISTER6, REGISTER7, REGISTER8>::state;

