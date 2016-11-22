/*! @file radio_config.h
 * @brief This file contains the automatically generated
 * configurations.
 *
 * @n WDS GUI Version: 3.2.8.0
 * @n Device: Si4455 Rev.: C2
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2015 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_CONFIG_H_
#define RADIO_CONFIG_H_

// USER DEFINED PARAMETERS
// Define your own parameters here

// INPUT DATA
/*
// Crys_freq(Hz): 26000000    Crys_tol(ppm): 30    IF_mode: 2    High_perf_Ch_Fil: 1    OSRtune: 0    Ch_Fil_Bw_AFC: 0    ANT_DIV: 0    PM_pattern: 0
// MOD_type: 2    Rsymb(sps): 2400    Fdev(Hz): 30000    RXBW(Hz): 114000    Manchester: 1    AFC_en: 0    Rsymb_error: 0.0    Chip-Version: 2
// RF Freq.(MHz): 434    API_TC: 28    fhst: 200000    inputBW: 0    BERT: 0    RAW_dout: 0    D_source: 0    Hi_pfm_div: 0
// API_ARR_Det_en: 0    Fdev_error: 0    API_ETSI: 0
//
// # WB filter 2 (BW = 119.09 kHz);  NB-filter 2 (BW = 119.09 kHz) 
//
// Modulation index: 25
*/


// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ                     26000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH               0x20
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP        0x03
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       0xF000


// CONFIGURATION COMMANDS

/*
// Command:                  RF_POWER_UP
// Description:              Command to power-up the device and select the operational mode and functionality.
*/
#define RF_POWER_UP 0x02, 0x81, 0x00, 0x01, 0x8C, 0xBA, 0x80

/*
// Set properties:           RF_INT_CTL_ENABLE_1
// Number of properties:     1
// Group ID:                 0x01
// Start ID:                 0x00
// Default values:           0x04,
// Descriptions:
//   INT_CTL_ENABLE - This property provides for global enabling of the three interrupt groups (Chip, Modem and Packet Handler) in order to generate HW interrupts at the NIRQ pin.
*/
#define RF_INT_CTL_ENABLE_1 0x11, 0x01, 0x01, 0x00, 0x00

/*
// Set properties:           RF_FRR_CTL_A_MODE_4
// Number of properties:     4
// Group ID:                 0x02
// Start ID:                 0x00
// Default values:           0x01, 0x02, 0x09, 0x00,
// Descriptions:
//   FRR_CTL_A_MODE - Fast Response Register A Configuration.
//   FRR_CTL_B_MODE - Fast Response Register B Configuration.
//   FRR_CTL_C_MODE - Fast Response Register C Configuration.
//   FRR_CTL_D_MODE - Fast Response Register D Configuration.
*/
#define RF_FRR_CTL_A_MODE_4 0x11, 0x02, 0x04, 0x00, 0x08, 0x06, 0x04, 0x0A

/*
// Set properties:           RF_EZCONFIG_XO_TUNE_1
// Number of properties:     1
// Group ID:                 0x24
// Start ID:                 0x03
// Default values:           0x40,
// Descriptions:
//   EZCONFIG_XO_TUNE - Configure the internal capacitor frequency tuning bank for the crystal oscillator.
*/
#define RF_EZCONFIG_XO_TUNE_1 0x11, 0x24, 0x01, 0x03, 0x52

/*
// Command:                  RF_WRITE_TX_FIFO
// Description:              Writes data byte(s) to the TX FIFO.
*/
#define RF_WRITE_TX_FIFO 0x66, 0x99, 0x5E, 0x42, 0xFF, 0x3B, 0xAD, 0x78, 0xD5, 0x1C, 0xBD, 0xED, 0x84, 0x1A, 0xF9, 0x51, 0x52, 0x77, 0xDD, 0xD4, \
0xD5, 0x38, 0xA9, 0x42, 0x6F, 0xB4, 0x4E, 0x3F, 0x35, 0xD6, 0xD7, 0x23, 0x1E, 0x72, 0x63, 0xBB, 0x76, 0xC7, 0xBE, 0x54, \
0x5E, 0xE5, 0x66, 0x24, 0xA0, 0xD9, 0x78, 0x08, 0x87, 0xA7, 0xC1, 0xF7, 0x2A, 0xA2, 0x9A, 0xC8, 0x44, 0x73, 0xB9, 0x97, \
0xA7, 0x54, 0x68, 0xB8, 0x89, 0xFE, 0x27, 0x73, 0xA1, 0xB4, 0x78, 0xA5, 0x6D, 0x3C, 0xE1, 0xA7, 0x79, 0xC4, 0x60, 0xEF, \
0xF8, 0x70, 0x3E, 0x8D, 0x07, 0xBD, 0xDF, 0x69, 0x13, 0xF9, 0x55, 0x30, 0xC6, 0xF4, 0x40, 0xC0, 0x1B, 0xA4, 0xE9, 0x19, \
0x59, 0x76, 0x91, 0x04, 0xFF, 0xF8, 0xE3, 0x0C, 0xCC, 0x82, 0x67, 0x42, 0x9C, 0x60

/*
// Command:                  RF_NOP
// Description:              No Operation command.
*/
#define RF_NOP 0x00

/*
// Command:                  RF_WRITE_TX_FIFO_1
// Description:              Writes data byte(s) to the TX FIFO.
*/
#define RF_WRITE_TX_FIFO_1 0x66, 0xB1, 0x41, 0xEF, 0x83, 0xBA, 0xB6, 0xF3, 0x17, 0x74, 0x7B, 0x77, 0xA8, 0xC8, 0x79, 0x40, 0xF8, 0x2C, 0xD6, 0xDB, \
0xDF, 0xFF, 0xE5, 0x19, 0xDC, 0xAF, 0x6C, 0x02, 0xB0, 0x84, 0x8F, 0x46, 0xC0, 0x6C, 0xEE, 0xC8, 0xD2, 0x19, 0x9D, 0xAD, \
0xA4, 0xF8, 0xE2, 0x70, 0x9D, 0x60, 0x23, 0x2A, 0xF4, 0xFF, 0xB4, 0x9F, 0x00, 0x83, 0x2E, 0x42, 0x9F, 0xC3, 0xEF, 0x48, \
0x26, 0x66, 0xA7, 0x2C, 0xC2, 0x65, 0xFC, 0xF9, 0xEA, 0x68, 0x2D, 0x1B, 0x44, 0x03, 0xBA, 0x35, 0xD5, 0x4B, 0x60, 0x55, \
0x40, 0x7C, 0xA9, 0x51, 0xE8, 0x9B, 0x01, 0xFF, 0xF9, 0x19, 0x6B, 0x9D, 0xE4, 0x48, 0x00, 0x14, 0xE3, 0x36, 0x99, 0x44, \
0x35, 0x26, 0xAF, 0x81, 0x12, 0x64, 0x1C, 0xFA, 0xAB, 0x6F, 0xD0, 0x2C

/*
// Command:                  RF_EZCONFIG_CHECK
// Description:              Validates the EZConfig array was written correctly.
*/
#define RF_EZCONFIG_CHECK 0x19, 0x8E, 0x5A

/*
// Command:                  RF_GPIO_PIN_CFG
// Description:              Configures the GPIO pins.
*/
#define RF_GPIO_PIN_CFG 0x13, 11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

#define GET_INT_STATUS 0x20, 0x31, 0x2f, 0x3c /*, 0x44, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff */


// AUTOMATICALLY GENERATED CODE!
// DO NOT EDIT/MODIFY BELOW THIS LINE!
// --------------------------------------------

#ifndef FIRMWARE_LOAD_COMPILE
#define RADIO_CONFIGURATION_DATA_ARRAY { \
        0x07, RF_POWER_UP, \
        0x05, RF_INT_CTL_ENABLE_1, \
        0x08, RF_FRR_CTL_A_MODE_4, \
        0x05, RF_EZCONFIG_XO_TUNE_1, \
        0x72, RF_WRITE_TX_FIFO, \
        0x01, RF_NOP, \
        0x70, RF_WRITE_TX_FIFO_1, \
        0x03, RF_EZCONFIG_CHECK, \
        0x08, RF_GPIO_PIN_CFG, \
        0x00 \
 }
#else
#define RADIO_CONFIGURATION_DATA_ARRAY { 0 }
#endif

// DEFAULT VALUES FOR CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ_DEFAULT                     30000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER_DEFAULT                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH_DEFAULT               0x10
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP_DEFAULT        0x01
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET_DEFAULT       0x1000

#define RADIO_CONFIGURATION_DATA_RADIO_PATCH_INCLUDED                      0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PATCH_SIZE                          0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PATCH                               {  }

#ifndef RADIO_CONFIGURATION_DATA_ARRAY
#error "This property must be defined!"
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ          RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ_DEFAULT
#endif

#ifndef RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER         RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER_DEFAULT
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH    RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH_DEFAULT
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP   RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP_DEFAULT
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET  RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET_DEFAULT
#endif

#define RADIO_CONFIGURATION_DATA { \
                            Radio_Configuration_Data_Array,                            \
                            RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,                   \
                            RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,              \
                            RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,       \
                            RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       \
                            }

#endif /* RADIO_CONFIG_H_ */
