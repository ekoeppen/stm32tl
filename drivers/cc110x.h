#ifndef __CC110X_H
#define __CC110X_H

#include <utils.h>

/**
 * Type of transfers
 */
#define WRITE_BURST              0x40
#define READ_SINGLE              0x80
#define READ_BURST               0xC0

/**
 * Type of register
 */
#define CC1101_CONFIG_REGISTER   READ_SINGLE
#define CC1101_STATUS_REGISTER   READ_BURST

/**
 * PATABLE & FIFO's
 */
#define CC1101_PATABLE           0x3E        // PATABLE address
#define CC1101_TXFIFO            0x3F        // TX FIFO address
#define CC1101_RXFIFO            0x3F        // RX FIFO address

/**
 * Command strobes
 */
#define CC1101_SRES              0x30        // Reset CC1101 chip
#define CC1101_SFSTXON           0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA):
                                             // Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define CC1101_SXOFF             0x32        // Turn off crystal oscillator
#define CC1101_SCAL              0x33        // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without
                                             // setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define CC1101_SRX               0x34        // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CC1101_STX               0x35        // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
                                             // If in RX state and CCA is enabled: Only go to TX if channel is clear
#define CC1101_SIDLE             0x36        // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#define CC1101_SWOR              0x38        // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if
                                             // WORCTRL.RC_PD=0
#define CC1101_SPWD              0x39        // Enter power down mode when CSn goes high
#define CC1101_SFRX              0x3A        // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC1101_SFTX              0x3B        // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC1101_SWORRST           0x3C        // Reset real time clock to Event1 value
#define CC1101_SNOP              0x3D        // No operation. May be used to get access to the chip status uint8_t

/**
 * CC1101 configuration registers
 */
#define CC1101_IOCFG2            0x00        // GDO2 Output Pin Configuration
#define CC1101_IOCFG1            0x01        // GDO1 Output Pin Configuration
#define CC1101_IOCFG0            0x02        // GDO0 Output Pin Configuration
#define CC1101_FIFOTHR           0x03        // RX FIFO and TX FIFO Thresholds
#define CC1101_SYNC1             0x04        // Sync Word, High Byte
#define CC1101_SYNC0             0x05        // Sync Word, Low Byte
#define CC1101_PKTLEN            0x06        // Packet Length
#define CC1101_PKTCTRL1          0x07        // Packet Automation Control
#define CC1101_PKTCTRL0          0x08        // Packet Automation Control
#define CC1101_ADDR              0x09        // Device Address
#define CC1101_CHANNR            0x0A        // Channel Number
#define CC1101_FSCTRL1           0x0B        // Frequency Synthesizer Control
#define CC1101_FSCTRL0           0x0C        // Frequency Synthesizer Control
#define CC1101_FREQ2             0x0D        // Frequency Control Word, High Byte
#define CC1101_FREQ1             0x0E        // Frequency Control Word, Middle Byte
#define CC1101_FREQ0             0x0F        // Frequency Control Word, Low Byte
#define CC1101_MDMCFG4           0x10        // Modem Configuration
#define CC1101_MDMCFG3           0x11        // Modem Configuration
#define CC1101_MDMCFG2           0x12        // Modem Configuration
#define CC1101_MDMCFG1           0x13        // Modem Configuration
#define CC1101_MDMCFG0           0x14        // Modem Configuration
#define CC1101_DEVIATN           0x15        // Modem Deviation Setting
#define CC1101_MCSM2             0x16        // Main Radio Control State Machine Configuration
#define CC1101_MCSM1             0x17        // Main Radio Control State Machine Configuration
#define CC1101_MCSM0             0x18        // Main Radio Control State Machine Configuration
#define CC1101_FOCCFG            0x19        // Frequency Offset Compensation Configuration
#define CC1101_BSCFG             0x1A        // Bit Synchronization Configuration
#define CC1101_AGCCTRL2          0x1B        // AGC Control
#define CC1101_AGCCTRL1          0x1C        // AGC Control
#define CC1101_AGCCTRL0          0x1D        // AGC Control
#define CC1101_WOREVT1           0x1E        // High Byte Event0 Timeout
#define CC1101_WOREVT0           0x1F        // Low Byte Event0 Timeout
#define CC1101_WORCTRL           0x20        // Wake On Radio Control
#define CC1101_FREND1            0x21        // Front End RX Configuration
#define CC1101_FREND0            0x22        // Front End TX Configuration
#define CC1101_FSCAL3            0x23        // Frequency Synthesizer Calibration
#define CC1101_FSCAL2            0x24        // Frequency Synthesizer Calibration
#define CC1101_FSCAL1            0x25        // Frequency Synthesizer Calibration
#define CC1101_FSCAL0            0x26        // Frequency Synthesizer Calibration
#define CC1101_RCCTRL1           0x27        // RC Oscillator Configuration
#define CC1101_RCCTRL0           0x28        // RC Oscillator Configuration
#define CC1101_FSTEST            0x29        // Frequency Synthesizer Calibration Control
#define CC1101_PTEST             0x2A        // Production Test
#define CC1101_AGCTEST           0x2B        // AGC Test
#define CC1101_TEST2             0x2C        // Various Test Settings
#define CC1101_TEST1             0x2D        // Various Test Settings
#define CC1101_TEST0             0x2E        // Various Test Settings

/**
 * Status registers
 */
#define CC1101_PARTNUM           0x30        // Chip ID
#define CC1101_VERSION           0x31        // Chip ID
#define CC1101_FREQEST           0x32        // Frequency Offset Estimate from Demodulator
#define CC1101_LQI               0x33        // Demodulator Estimate for Link Quality
#define CC1101_RSSI              0x34        // Received Signal Strength Indication
#define CC1101_MARCSTATE         0x35        // Main Radio Control State Machine State
#define CC1101_WORTIME1          0x36        // High Byte of WOR Time
#define CC1101_WORTIME0          0x37        // Low Byte of WOR Time
#define CC1101_PKTSTATUS         0x38        // Current GDOx Status and Packet Status
#define CC1101_VCO_VC_DAC        0x39        // Current Setting from PLL Calibration Module
#define CC1101_TXBYTES           0x3A        // Underflow and Number of Bytes
#define CC1101_RXBYTES           0x3B        // Overflow and Number of Bytes
#define CC1101_RCCTRL1_STATUS    0x3C        // Last RC Oscillator Calibration Result
#define CC1101_RCCTRL0_STATUS    0x3D        // Last RC Oscillator Calibration Result

static constexpr uint8_t cc110x_default_init_values[][2] = {
	{CC1101_IOCFG0,      0x41}, // CHIP_RDYn
	{CC1101_IOCFG2,      0x06}, // IRQ
	{CC1101_FIFOTHR,     0x47}, // RX attenuation 6dB, 33/32 byte threshold
	{CC1101_PKTLEN,      0x3D}, // 62 bytes max packet length
	{CC1101_PKTCTRL1,    0x0C}, // CRC autoflush, status append
	{CC1101_PKTCTRL0,    0x05}, // TX/RX CRC enabled, variable packet length
	{CC1101_FSCTRL1,     0x06}, // 152kHz IF frequency
	{CC1101_FREQ2,       0x10}, // 434 MHz carrier frequency
	{CC1101_FREQ1,       0xB1},
	{CC1101_FREQ0,       0x3B},
	{CC1101_MDMCFG4,     0xFA}, // 135kHz channel bandwidth
	{CC1101_MDMCFG3,     0x83}, // 38.4kbps symbol rate
	{CC1101_MDMCFG2,     0x31}, // OOK, 16/16 sync word detection
	{CC1101_MDMCFG1,     0x42}, // 8 bytes preamble
	{CC1101_DEVIATN,     0x27}, // 11.9kHz FSK deviation
	{CC1101_MCSM1,       0x3c},
	{CC1101_MCSM0,       0x18},
	{CC1101_FOCCFG,      0x16},
	{CC1101_WORCTRL,     0xFB},
	{CC1101_FSCAL3,      0xE9},
	{CC1101_FSCAL2,      0x2A},
	{CC1101_FSCAL1,      0x00},
	{CC1101_FSCAL0,      0x1F},
	{CC1101_AGCCTRL2,    0x04},
	{CC1101_AGCCTRL1,    0x00},
	{CC1101_AGCCTRL0,    0x91},
	{CC1101_TEST2,       0x81},
	{CC1101_TEST1,       0x35},
	{CC1101_TEST0,       0x09},
};

template<typename SPI,
	typename CSN,
	typename MISO,
	typename IRQ,
	typename DELAY_TIMER,
	const uint8_t CHANNEL,
	const uint16_t SYNC_WORD = 0xd391,
	const uint8_t ADDRESS = 0>
struct CC110X_T {
	static uint8_t status_byte;

	static void reset(void) {
		CSN::set_high();
		DELAY_TIMER::set_and_wait(5);
		CSN::set_low();
		DELAY_TIMER::set_and_wait(10);
		CSN::set_high();
		DELAY_TIMER::set_and_wait(41);
		CSN::set_low();
		if (!MISO::is_unused) {
			while (MISO::is_high());
		}
		SPI::transfer(CC1101_SRES);
		if (!MISO::is_unused) {
			while (MISO::is_high());
		} else {
			DELAY_TIMER::set_and_wait_us(40);
		}
		CSN::set_high();
	}

	static void init(const uint8_t reg_values[][2], uint8_t num_values) {
		for (int i = 0; i < num_values; i++) {
			write_reg(reg_values[i][0], reg_values[i][1]);
		}
	}

	static void disable(void) {
	}

	static void write_burst_reg(uint8_t addr, const uint8_t *buffer, uint8_t len) {
		CSN::set_low();
		SPI::transfer(WRITE_BURST | addr);
		SPI::transfer((uint8_t *) buffer, len, 0);
		CSN::set_high();
	}

	static uint8_t read_reg(uint8_t addr) {
		uint8_t r;
		CSN::set_low();
		status_byte = SPI::transfer(addr);
		r = SPI::transfer(0xff);
		CSN::set_high();
		return r;
	}

	static void read_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t len) {
		CSN::set_low();
		SPI::transfer(READ_BURST | addr);
		SPI::transfer(0, len, (uint8_t *) buffer);
		CSN::set_high();
	}

	static void write_reg(uint8_t addr, uint8_t value) {
		CSN::set_low();
		status_byte = SPI::transfer(addr);
		SPI::transfer(value);
		CSN::set_high();
	}

	static void power_down(void) {
		CSN::set_low();
		SPI::transfer(CC1101_SIDLE);
		status_byte = SPI::transfer(CC1101_SPWD);
		CSN::set_high();
	}

	static void power_up(void) {
		CSN::set_low();
		DELAY_TIMER::set_and_wait(50);
		write_reg(CC1101_IOCFG2, 0x06);
	}

	static void start_tx(void) {
	}

	static void start_rx(void) {
		CSN::set_low();
		SPI::transfer(CC1101_SIDLE);
		SPI::transfer(CC1101_SFRX);
		status_byte = SPI::transfer(CC1101_SRX);
		CSN::set_high();
	}

	static void tx_buffer(const uint8_t *data, const uint8_t len) {
		write_reg(CC1101_TXFIFO, len);
		write_burst_reg(CC1101_TXFIFO, data, len);
		CSN::set_low();
		status_byte = SPI::transfer(CC1101_STX);
		CSN::set_high();
		IRQ::wait_for_irq();
		IRQ::clear_irq();
	}

	template<typename RX_TIMEOUT = TIMEOUT_NEVER>
	static int rx_buffer(uint8_t *data, int max_len) {
		uint8_t n = 0;
		if (IRQ::template wait_for_irq<RX_TIMEOUT>()) {
			if ((n = read_reg(CC1101_STATUS_REGISTER | CC1101_RXBYTES)) > 0) {
				if (n > max_len) n = max_len;
				read_burst_reg(CC1101_RXFIFO, data, n);
			}
			IRQ::clear_irq();
		}
		return n;
	}

	static void read_regs(uint8_t *buffer) {
		int i, n;

		for (n = 0, i = 0x00; i < 0x30; i++) {
			buffer[n++] = read_reg(CC1101_CONFIG_REGISTER | i);
		}
		for (i = 0x30; i < 0x3e; i++) {
			buffer[n++] = read_reg(CC1101_STATUS_REGISTER | i);
		}
	}
};

template<typename SPI, typename CSN, typename MISO, typename IRQ, typename DELAY_TIMER, const uint8_t CHANNEL, const uint16_t SYNC_WORD, const uint8_t ADDRESS>
uint8_t CC110X_T<SPI, CSN, MISO, IRQ, DELAY_TIMER, CHANNEL, SYNC_WORD, ADDRESS>::status_byte;

#endif
