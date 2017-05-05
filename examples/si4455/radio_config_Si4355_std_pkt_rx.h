/*! @file radio_config.h
 * @brief This file contains the automatically generated
 * configurations.
 *
 * @n WDS GUI Version: 3.2.11.0
 * @n Device: Si4355 Rev.: B1                                 
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2017 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_CONFIG_H_
#define RADIO_CONFIG_H_

// USER DEFINED PARAMETERS
// Define your own parameters here

// INPUT DATA
/*
// Crys_freq(Hz): 26000000    Crys_tol(ppm): 30    IF_mode: 2    High_perf_Ch_Fil: 1    OSRtune: 0    Ch_Fil_Bw_AFC: 0    ANT_DIV: 0    PM_pattern: 0    
// MOD_type: 1    Rsymb(sps): 38400    Fdev(Hz): 384000    RXBW(Hz): 185000    Manchester: 0    AFC_en: 1    Rsymb_error: 0.0    Chip-Version: 2    
// RF Freq.(MHz): 434    API_TC: 28    fhst: 250000    inputBW: 0    BERT: 0    RAW_dout: 0    D_source: 0    Hi_pfm_div: 0    
// 
// # WB filter 1 (BW = 198.40 kHz);  NB-filter 1 (BW = 198.40 kHz) 
// 
// Modulation index: 20
*/


// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ                     26000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH               0x07
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP        0x03
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       0xF000


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
#define RF_INT_CTL_ENABLE_2 0x11, 0x01, 0x02, 0x00, 0x05, 0x18

/*
// Set properties:           RF_INT_CTL_CHIP_ENABLE_1
// Number of properties:     1
// Group ID:                 0x01
// Start ID:                 0x03
// Default values:           0x04, 
// Descriptions:
//   INT_CTL_CHIP_ENABLE - Enable individual interrupt sources within the Chip Interrupt Group to generate a HW interrupt on the NIRQ output pin.
*/
#define RF_INT_CTL_CHIP_ENABLE_1 0x11, 0x01, 0x01, 0x03, 0x04

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
#define RF_EZCONFIG_ARRAY_WRITE 0x66, 0xC6, 0x90, 0xEA, 0x8C, 0xDB, 0xCE, 0x78, 0xB7, 0x56, 0x48, 0x6B, 0xD2, 0x46, 0xEC, 0xFA, 0x37, 0xA8, 0x4B, 0x67, \
0x86, 0xA5, 0x04, 0x6E, 0x6C, 0x5C, 0x3E, 0x67, 0x75, 0x9D, 0x72, 0x45, 0x12, 0x0E, 0x46, 0x61, 0x7B, 0x39, 0xA1, 0x12, \
0xC5, 0x06, 0xD1, 0xDA, 0x6B, 0xE2, 0x16, 0xE3, 0x96, 0x05, 0x68, 0xCC, 0x6C, 0x37, 0x75, 0xA2, 0x9C, 0xDA, 0x10, 0x5D, \
0x65, 0x62, 0x5F, 0xBB, 0xEE, 0x7A, 0x15, 0x2B, 0xDB, 0x27, 0x0E, 0xFE, 0x8C, 0x8A, 0x32, 0xBC, 0x8B, 0xB5, 0x76, 0xB8, \
0x8C, 0xB6, 0x9B, 0xCF, 0x79, 0x09, 0x28, 0x68, 0x2B, 0x72, 0xF5, 0xCA, 0x64, 0x1D, 0xD5, 0x70, 0x31, 0x15, 0xD3, 0x11, \
0x3C, 0x46, 0x52, 0x8B, 0x8B, 0xE5, 0x91, 0x6E, 0xA5, 0xBB, 0xD8, 0x66, 0xEC, 0x26

/*
// Command:                  RF_NOP
// Description:              No Operation command.
*/
#define RF_NOP 0x00

/*
// Command:                  RF_EZCONFIG_ARRAY_WRITE_1
// Description:              Writes data byte(s) to the EZConfig array.
*/
#define RF_EZCONFIG_ARRAY_WRITE_1 0x66, 0xF8, 0x8D, 0x92, 0x1E, 0x3A, 0xA3, 0xBC, 0x6B, 0x08, 0x70, 0x4E, 0x04, 0xB3, 0x91, 0x37, 0x10, 0x87, 0xD5, 0x94, \
0x71, 0xB5, 0xB2, 0x5B, 0xAB, 0x12, 0xD9, 0x37, 0xBA, 0x06, 0xBF, 0xE0, 0xBE, 0x8E, 0x49, 0xCF, 0x5D, 0xA1, 0x2C, 0x80, \
0xEF, 0xAF, 0xA5, 0x9A, 0x7A, 0x47, 0xA0, 0xF7, 0x30, 0xA5, 0xF3, 0xF8, 0xCF, 0xB9, 0x04, 0x2E, 0x08, 0xDC, 0xD9, 0xBD, \
0xAF, 0x6B, 0xB3, 0x79, 0x47, 0x8A, 0x86, 0x88, 0xDA, 0xE4, 0xA3, 0x3E, 0xBF, 0x04, 0xBB, 0x27, 0xB6, 0x6E, 0xFB, 0xDD, \
0x29, 0x39, 0x44, 0xAB, 0x9A, 0xAB, 0x87, 0x4D, 0x48, 0x10, 0xF2, 0xFE, 0xBB, 0xC6, 0x6F, 0x5B, 0x26, 0x00, 0xB3, 0x00, \
0x54, 0xFA, 0xC3, 0x30, 0xDD, 0x69, 0x9F, 0x98, 0xE9, 0x2E, 0xBF, 0xC6

/*
// Command:                  RF_EZCONFIG_CHECK
// Description:              Validates the EZConfig array was written correctly.
*/
#define RF_EZCONFIG_CHECK 0x19, 0xC6, 0x5D

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
        0x05, RF_INT_CTL_CHIP_ENABLE_1, \
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
