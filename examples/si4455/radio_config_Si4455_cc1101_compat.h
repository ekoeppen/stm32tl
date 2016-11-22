/*! @file radio_config.h
 * @brief This file contains the automatically generated
 * configurations.
 *
 * @n WDS GUI Version: 3.2.9.0
 * @n Device: Si4455 Rev.: B1
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
// MOD_type: 2    Rsymb(sps): 38400    Fdev(Hz): 11900    RXBW(Hz): 114000    Manchester: 0    AFC_en: 0    Rsymb_error: 0.0    Chip-Version: 2
// RF Freq.(MHz): 434    API_TC: 28    fhst: 135000    inputBW: 0    BERT: 0    RAW_dout: 0    D_source: 0    Hi_pfm_div: 0
//
// # WB filter 2 (BW =  89.32 kHz);  NB-filter 2 (BW = 89.32 kHz) 
//
// Modulation index: 0
*/


// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ                     26000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH               0x05
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP        0x03
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       0xF000
#define RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE    		   0x00
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD					   {0xC5, 0xC5, 0xC5, 0xC5, 0xC5}


// CONFIGURATION COMMANDS

/*
// Command:                  RF_POWER_UP
// Description:              Command to power-up the device and select the operational mode and functionality.
*/
#define RF_POWER_UP 0x02, 0x01, 0x00, 0x01, 0x8C, 0xBA, 0x80

/*
// Set properties:           RF_INT_CTL_ENABLE_2
// Number of properties:     2
// Group ID:                 0x01
// Start ID:                 0x00
// Default values:           0x04, 0x00,
// Descriptions:
//   INT_CTL_ENABLE - This property provides for global enabling of the three interrupt groups (Chip, Modem and Packet Handler) in order to generate HW interrupts at the NIRQ pin.
//   INT_CTL_PH_ENABLE - Enable individual interrupt sources within the Packet Handler Interrupt Group to generate a HW interrupt on the NIRQ output pin.
*/
#define RF_INT_CTL_ENABLE_2 0x11, 0x01, 0x02, 0x00, 0x01, 0x38

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
#define RF_WRITE_TX_FIFO 0x66, 0xA0, 0xC4, 0x14, 0x75, 0xC3, 0x82, 0x8C, 0x22, 0x1A, 0x92, 0x10, 0x61, 0x57, 0x03, 0xE3, 0xE8, 0xC1, 0xF5, 0x95, \
0x22, 0x54, 0xFC, 0x57, 0xEA, 0xD8, 0x3B, 0x63, 0x93, 0xDE, 0xE3, 0xA4, 0x34, 0x9F, 0xF8, 0x80, 0xBA, 0xC4, 0x20, 0xBF, \
0x22, 0xB0, 0x3B, 0x17, 0xBF, 0xBD, 0x2B, 0x53, 0xAC, 0xF8, 0xFF, 0x36, 0x01, 0xC0, 0x2A, 0x50, 0x3C, 0x30, 0x32, 0xEE, \
0x8C, 0xBD, 0xE4, 0xE3, 0xA8, 0x31, 0x3B, 0x7F, 0x19, 0x02, 0x85, 0xEB, 0xCD, 0x72, 0xAB, 0xF3, 0x73, 0x06, 0xD3, 0xDB, \
0x66, 0x86, 0x4E, 0x9D, 0x6D, 0x94, 0x1B, 0x8C, 0xD9, 0x0A, 0xCA, 0x51, 0xCD, 0x8E, 0x69, 0x2C, 0x8A, 0xA6, 0x8C, 0x41, \
0xDB, 0xA9, 0x50, 0x4C, 0xC0, 0xC3, 0xF4, 0xEE, 0xF8, 0x7A, 0xC0, 0x8C, 0xE5, 0xD4

/*
// Command:                  RF_NOP
// Description:              No Operation command.
*/
#define RF_NOP 0x00

/*
// Command:                  RF_WRITE_TX_FIFO_1
// Description:              Writes data byte(s) to the TX FIFO.
*/
#define RF_WRITE_TX_FIFO_1 0x66, 0x1C, 0x18, 0xDD, 0xFF, 0x1B, 0x7E, 0x80, 0x8B, 0x15, 0x7A, 0x31, 0x35, 0x14, 0xEA, 0xD7, 0x4C, 0x4F, 0xA7, 0xBE, \
0xAD, 0x84, 0xD0, 0xB4, 0x83, 0x83, 0x40, 0x07, 0x46, 0xA4, 0x77, 0x99, 0x1E, 0x69, 0xF0, 0xA8, 0x4D, 0x12, 0xE0, 0x99, \
0x1E, 0xD9, 0xB2, 0xD6, 0xAC, 0x1E, 0x88, 0xD1, 0x12, 0xBE, 0x66, 0xD0, 0x46, 0x61, 0xFA, 0xFC, 0xE3, 0x49, 0xD4, 0xE2, \
0x43, 0xCD, 0x06, 0x16, 0x91, 0x1A, 0x1B, 0xA3, 0x64, 0x9A, 0x29, 0x0A, 0x34, 0x8C, 0xB1, 0x2B, 0xA8, 0x02, 0xD7, 0x5B, \
0xD0, 0x31, 0x6E, 0x68, 0xEC, 0x13, 0x2A, 0x3C, 0x8A, 0xBC, 0x9F, 0x7D, 0x99, 0x06, 0xAA, 0x4C, 0xC3, 0x29, 0x1D, 0x17, \
0xC8, 0xC0, 0x1D, 0x62, 0xF9, 0x54, 0xCC, 0xF6, 0x6B, 0x45, 0xC6, 0xAC

/*
// Command:                  RF_EZCONFIG_CHECK
// Description:              Validates the EZConfig array was written correctly.
*/
#define RF_EZCONFIG_CHECK 0x19, 0x2C, 0x3E

/*
// Command:                  RF_GPIO_PIN_CFG
// Description:              Configures the GPIO pins.
*/
#define RF_GPIO_PIN_CFG 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00


// AUTOMATICALLY GENERATED CODE!
// DO NOT EDIT/MODIFY BELOW THIS LINE!
// --------------------------------------------

#ifndef FIRMWARE_LOAD_COMPILE
#define RADIO_CONFIGURATION_DATA_ARRAY { \
        0x07, RF_POWER_UP, \
        0x06, RF_INT_CTL_ENABLE_2, \
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
#define RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE_DEFAULT    	       0x00
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD_DEFAULT  		           {0x42, 0x55, 0x54, 0x54, 0x4F, 0x4E, 0x31} // BUTTON1

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

#ifndef RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE
#define RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE  RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE_DEFAULT
#endif

#ifndef RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD         RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD_DEFAULT
#endif

#define RADIO_CONFIGURATION_DATA { \
                            Radio_Configuration_Data_Array,                            \
                            RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,                   \
                            RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,              \
                            RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,       \
                            RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET,      \
                            RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE,              \
                            RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD                    \
                            }

#endif /* RADIO_CONFIG_H_ */
