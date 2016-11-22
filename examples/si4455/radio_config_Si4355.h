/*! @file radio_config.h
 * @brief This file contains the automatically generated
 * configurations.
 *
 * @n WDS GUI Version: 3.2.9.0
 * @n Device: Si4355 Rev.: B1
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
// MOD_type: 2    Rsymb(sps): 9600    Fdev(Hz): 59000    RXBW(Hz): 114000    Manchester: 0    AFC_en: 0    Rsymb_error: 0.0    Chip-Version: 2
// RF Freq.(MHz): 433.92    API_TC: 28    fhst: 250000    inputBW: 0    BERT: 0    RAW_dout: 0    D_source: 0    Hi_pfm_div: 0
//
// # WB filter 2 (BW = 178.64 kHz);  NB-filter 2 (BW = 178.64 kHz) 
//
// Modulation index: 12
*/


// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ                     26000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH               0x07
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP        0x03
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       0xF000
#define RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE    		   0x00
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD					   {0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5}


// CONFIGURATION COMMANDS

/*
// Command:                  RF_POWER_UP
// Description:              Command to power-up the device and select the operational mode and functionality.
*/
#define RF_POWER_UP 0x02, 0x01, 0x00, 0x01, 0x8C, 0xBA, 0x80

/*
// Set properties:           RF_INT_CTL_ENABLE_4
// Number of properties:     4
// Group ID:                 0x01
// Start ID:                 0x00
// Default values:           0x04, 0x00, 0x00, 0x04,
// Descriptions:
//   INT_CTL_ENABLE - This property provides for global enabling of the three interrupt groups (Chip, Modem and Packet Handler) in order to generate HW interrupts at the NIRQ pin.
//   INT_CTL_PH_ENABLE - Enable individual interrupt sources within the Packet Handler Interrupt Group to generate a HW interrupt on the NIRQ output pin.
//   INT_CTL_MODEM_ENABLE - Enable individual interrupt sources within the Modem Interrupt Group to generate a HW interrupt on the NIRQ output pin.
//   INT_CTL_CHIP_ENABLE - Enable individual interrupt sources within the Chip Interrupt Group to generate a HW interrupt on the NIRQ output pin.
*/
#define RF_INT_CTL_ENABLE_4 0x11, 0x01, 0x04, 0x00, 0x05, 0x10, 0x00, 0x04

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
// Command:                  RF_EZCONFIG_ARRAY_WRITE
// Description:              Writes data byte(s) to the EZConfig array.
*/
#define RF_EZCONFIG_ARRAY_WRITE 0x66, 0xF1, 0x21, 0x4F, 0xF7, 0x16, 0x7F, 0x70, 0x2D, 0x33, 0x43, 0x0A, 0xA2, 0x2F, 0x97, 0x48, 0x6C, 0x2A, 0x94, 0xA2, \
0x7E, 0x3A, 0xA5, 0xEF, 0xA6, 0xEC, 0x24, 0x97, 0x48, 0x03, 0x0D, 0x0E, 0x2C, 0x36, 0x54, 0x94, 0x2A, 0xE6, 0x86, 0x0D, \
0x50, 0x32, 0xE4, 0xDF, 0x2F, 0x48, 0xD3, 0x64, 0x16, 0x56, 0x87, 0x4A, 0x3E, 0x51, 0xCB, 0x51, 0x37, 0xEA, 0x2D, 0xB5, \
0x12, 0x2C, 0xCA, 0x68, 0xF8, 0x6F, 0x08, 0x03, 0x65, 0x53, 0xB8, 0x7D, 0x3D, 0x2C, 0x48, 0x77, 0xAD, 0xDD, 0x66, 0xB0, \
0x49, 0x12, 0xDB, 0x91, 0x31, 0x7E, 0xA9, 0x55, 0xA6, 0xD2, 0x1B, 0x6E, 0xA0, 0x67, 0x7D, 0x21, 0xD3, 0x54, 0x7A, 0x50, \
0x8A, 0x2C, 0xA4, 0x6F, 0x34, 0x34, 0xF2, 0xC1, 0x01, 0x84, 0xD4, 0xF3, 0x90, 0xE4

/*
// Command:                  RF_NOP
// Description:              No Operation command.
*/
#define RF_NOP 0x00

/*
// Command:                  RF_EZCONFIG_ARRAY_WRITE_1
// Description:              Writes data byte(s) to the EZConfig array.
*/
#define RF_EZCONFIG_ARRAY_WRITE_1 0x66, 0x8B, 0xCF, 0x5D, 0x01, 0x1A, 0x7A, 0x9B, 0xC9, 0xC9, 0x38, 0x7C, 0x1E, 0x49, 0xD1, 0x8F, 0x09, 0x07, 0x80, 0xDF, \
0x36, 0x94, 0xEF, 0x5C, 0x7B, 0xEF, 0x3B, 0x7C, 0x4F, 0x81, 0x90, 0x0B, 0x81, 0x0B, 0x0B, 0x54, 0x84, 0xAF, 0xD1, 0xBE, \
0x6A, 0x39, 0xCC, 0x9B, 0x0F, 0x73, 0x29, 0x7F, 0x35, 0x88, 0x00, 0x32, 0x5B, 0x65, 0xAA, 0x07, 0x1C, 0x51, 0x8C, 0x6E, \
0xBE, 0xE7, 0xFB, 0xE8, 0x9E, 0x3C, 0xEB, 0x54, 0x79, 0xEC, 0x55, 0x33, 0xAA, 0x52, 0x38, 0xDC, 0xA0, 0x0F, 0xC5, 0x10, \
0x05, 0x7A, 0x2A, 0x4A, 0xB8, 0x21, 0x7B, 0x70, 0xCB, 0x92, 0xF7, 0xEF, 0x80, 0x30, 0x4B, 0xBD, 0xA6, 0x5B, 0x6F, 0x17, \
0xE4, 0x2E, 0x92, 0x2B, 0x61, 0xB7, 0xDA, 0xC7, 0xB5, 0x2E, 0x77, 0x70

/*
// Command:                  RF_EZCONFIG_CHECK
// Description:              Validates the EZConfig array was written correctly.
*/
#define RF_EZCONFIG_CHECK 0x19, 0xF1, 0x56

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
        0x08, RF_INT_CTL_ENABLE_4, \
        0x08, RF_FRR_CTL_A_MODE_4, \
        0x05, RF_EZCONFIG_XO_TUNE_1, \
        0x72, RF_EZCONFIG_ARRAY_WRITE, \
        0x01, RF_NOP, \
        0x70, RF_EZCONFIG_ARRAY_WRITE_1, \
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
