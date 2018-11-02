#pragma once

namespace USB_T {

enum STANDARD_REQUESTS {
	GET_STATUS = 0,
	CLEAR_FEATURE = 1,
	SET_FEATURE = 3,
	SET_ADDRESS = 5,
	GET_DESCRIPTOR = 6,
	SET_DESCRIPTOR = 7,
	GET_CONFIGURATION = 8,
	SET_CONFIGURATION = 9,
	GET_INTERFACE = 10,
	SET_INTERFACE = 11,
	SYNCH_FRAME = 12
};

enum DESCRIPTOR_TYPE {
	DEVICE_DESCRIPTOR = 1,
	CONFIG_DESCRIPTOR = 2,
	STRING_DESCRIPTOR = 3,
	INTERFACE_DESCRIPTOR = 4,
	ENDPOINT_DESCRIPTOR = 5
};

enum FEATURE_SELECTOR {
	ENDPOINT_STALL,
	DEVICE_REMOTE_WAKEUP
};

#define REQUEST_TYPE      0x60  /* Mask to get request type */
#define STANDARD_REQUEST  0x00  /* Standard request */
#define CLASS_REQUEST     0x20  /* Class request */
#define VENDOR_REQUEST    0x40  /* Vendor request */
#define RECIPIENT         0x1F  /* Mask to get recipient */

enum EP_TYPE {
	BULK = 0,
	CONTROL = 1,
	ISO = 2,
	INTERRUPT = 3
};

enum EP_STATUS {
	DISABLED = 0,
	STALL = 1,
	NAK = 2,
	VALID = 3
};

struct USB_TypeDef {
	__IO uint32_t EP0R;
	__IO uint32_t EP1R;
	__IO uint32_t EP2R;
	__IO uint32_t EP3R;
	__IO uint32_t EP4R;
	__IO uint32_t EP5R;
	__IO uint32_t EP6R;
	__IO uint32_t EP7R;
	uint32_t  RESERVED0;
	uint32_t  RESERVED1;
	uint32_t  RESERVED2;
	uint32_t  RESERVED3;
	uint32_t  RESERVED4;
	uint32_t  RESERVED5;
	uint32_t  RESERVED6;
	uint32_t  RESERVED7;
	__IO uint32_t CNTR;
	__IO uint32_t ISTR;
	__IO uint32_t FNR;
	__IO uint32_t DADDR;
	__IO uint32_t BTABLE;
};

struct USB_BTABLE_TypeDef {
	uint16_t USB_ADDR_TX;
	uint16_t RESERVED0;
	uint16_t USB_COUNT_TX;
	uint16_t RESERVED1;
	uint16_t USB_ADDR_RX;
	uint16_t RESERVED2;
	uint16_t USB_COUNT_RX;
	uint16_t RESERVED3;
};

struct SETUP_PACKET_HEADER {
	uint8_t bmRequestType;
	uint8_t bRequest;
};

struct SETUP_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wValue;
	uint16_t RESERVED1;
	uint16_t wIndex;
	uint16_t RESERVED2;
	uint16_t wLength;
};

struct CLEAR_FEATURE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wFeature;
	uint16_t RESERVED1;
	uint16_t wIndex;
};

struct GET_CONFIGURATION_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wFeature;
	uint16_t RESERVED1;
	uint16_t wIndex;
	uint16_t RESERVED2;
	uint16_t wLength;
	uint16_t RESERVED3;
	uint8_t bConfiguration;
};

struct GET_DESCRIPTOR_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint8_t bDescriptorIndex;
	uint8_t bDescriptorType;
	uint16_t RESERVED1;
	uint16_t wLanguageID;
	uint16_t RESERVED2;
	uint16_t wLength;
};

struct GET_INTERFACE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wInterface;
	uint16_t wLength;
	uint8_t bAlternateInterface;
};

struct GET_STATUS_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t wStatus;
};

struct SET_ADDRESS_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wAddress;
};

struct SET_CONFIGURATION_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wConfiguration;
};

struct SET_DESCRIPTOR_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t bDescriptorIndex;
	uint8_t bDescriptorType;
	uint16_t wLanguageID;
	uint16_t wLength;
};

struct SET_FEATURE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wFeatureSelector;
	uint16_t wIndex;
};

struct SET_INTERFACE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wAlternateSetting;
	uint16_t wInterface;
};

struct SYNC_FRAME {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wEndpoint;
	uint16_t wLength;
	uint16_t wFrameNumber;
};

#define USB_BTABLE_BASE 0x40006000
USB_BTABLE_TypeDef USB_BTABLE[8] __attribute__((section(".usb_pma_peripheral")));
// USB_BTABLE_TypeDef * const USB_BTABLE = (USB_BTABLE_TypeDef *) &USB_BTABLE_P[0];

}
