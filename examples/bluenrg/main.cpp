#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <utils.h>
#include <spi.h>
#include <exti.h>

#include <board.h>

extern "C" {

#include <compiler.h>
#include <hci.h>
#include <hci_const.h>
#include <bluenrg_gap.h>
#include <sm.h>
#include <bluenrg_aci.h>
#include <bluenrg_aci_const.h>
#include <bluenrg_gap_aci.h>
#include <bluenrg_hal_aci.h>
#include <bluenrg_gatt_aci.h>
#include "SDK_EVAL_Spi_Driver.h"

}

uint8_t bdaddr[] = {0xff, 0x00, 0x00, 0xE1, 0x80, 0x02};
const uint32_t CLOCK_SECOND = 1000;
volatile uint32_t ticks = 0;

uint16_t connection_handle;
uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
uint16_t battery_svc_handle, battery_char_handle;
uint8_t battery_level = 50;

typedef PLL_T<HSI_RC, 48000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;
typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON | BOARD_T::SPI> BOARD;

typedef GPIO_T<PB, 1, MODE_INPUT, TYPE_PUSH_PULL, SPEED_LOW, PULL_DOWN, AF0, EDGE_RISING> BLUENRG_IRQ;
typedef GPIO_OUTPUT_T<PA, 8> BLUENRG_RESET;
typedef GPIO_OUTPUT_T<PA, 4> BLUENRG_CS;
typedef GPIO_OUTPUT_T<PB, 0> P3;

typedef SPI_T<SYSCLK, true, 0, 1000000> SPI;
typedef EXTI_T<BLUENRG_IRQ, BUTTON> EXT_INTERRUPT;

uint8_t SdkEvalSPI_Irq_Pin(void) {
	return BLUENRG_IRQ::get_value();
}

void Clear_SPI_EXTI_Flag() {
	BLUENRG_IRQ::clear_irq();
}

void Enable_SPI_IRQ(void) {
	BLUENRG_IRQ::enable_irq();
}

void Disable_SPI_IRQ(void) {
	BLUENRG_IRQ::disable_irq();
}

void BlueNRG_RST(void) {
	BLUENRG_RESET::set_low();
	TIMEOUT::set_and_wait(5);
	BLUENRG_RESET::set_high();
	TIMEOUT::set_and_wait(5);
}

bool BlueNRG_Ready(void) {
	const uint8_t header_master[5] = {0x0b, 0x00, 0x00, 0x00, 0x00};
	uint8_t header_slave[5];

	BLUENRG_CS::set_low();
	SPI::transfer(header_master, sizeof(header_master), header_slave);
	BLUENRG_CS::set_high();

	return header_slave[0] == 0x02;
}

uint8_t BlueNRG_SPI_Read_All(uint8_t *buffer, uint8_t buff_size) {
	uint16_t byte_count;
	uint8_t len = 0;
	const uint8_t header_master[5] = {0x0b, 0x00, 0x00, 0x00, 0x00};
	uint8_t header_slave[5];

	BLUENRG_IRQ::disable_irq();
	BLUENRG_CS::set_low();
	SPI::transfer(header_master, sizeof(header_master), header_slave);

	if (header_slave[0] == 0x02) {
		byte_count = (header_slave[4]<<8)|header_slave[3];
		if (byte_count > 0) {
			if (byte_count > buff_size) byte_count = buff_size;
			SPI::transfer(0, byte_count, buffer);
			len = byte_count;
		}
	}
	BLUENRG_CS::set_high();
	BLUENRG_IRQ::enable_irq();

	return len;
}

extern "C" {

tClockTime Clock_Time(void) {
	return ticks;
}

int16_t Hal_Write_Serial(uint8_t* data1, uint8_t* data2, uint16_t Nb_bytes1, uint16_t Nb_bytes2) {
	int16_t result = 0;
	uint16_t tx_bytes;
	uint8_t rx_bytes;
	const uint8_t header_master[5] = {0x0a, 0x00, 0x00, 0x00, 0x00};
	uint8_t header_slave[5]  = {0x00};


	BLUENRG_IRQ::disable_irq();
	while (1) {
		BLUENRG_CS::set_low();
		SPI::transfer(header_master, sizeof(header_master), header_slave);
		if (header_slave[0] == 0x02) {
			rx_bytes = header_slave[1];
			if (rx_bytes >= Nb_bytes1) break;
		}
		BLUENRG_CS::set_high();
	}
	SPI::transfer(data1, Nb_bytes1);

	rx_bytes -= Nb_bytes1;
	if (Nb_bytes2 > rx_bytes){
		tx_bytes = rx_bytes;
	} else {
		tx_bytes = Nb_bytes2;
	}

	SPI::transfer(data2, tx_bytes);
	result = tx_bytes;

	BLUENRG_CS::set_high();
	BLUENRG_IRQ::enable_irq();
	return result;
}

void EXTI0_1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0, 1>();
	if (BLUENRG_IRQ::irq_raised()) {
		P3::set_high();
		BLUENRG_IRQ::clear_irq();
		HCI_Isr();
		P3::set_low();
	}
	exit_idle();
}

void EXTI2_3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2, 3>();
	exit_idle();
}

void EXTI4_15_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4, 15>();
	exit_idle();
}

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
	ticks++;
}

}

void make_discoverable(void)
{
	const uint8_t adv_data[] = {
		/* Advertising data: Flags AD Type */
		0x02,
		0x01,
		0x06,
		/* Advertising data: manufacturer specific data */
		26, AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
		0x30, 0x00, //Company identifier code (Default is 0x0030 - STMicroelectronics: To be customized for specific identifier)
		0x02,       // ID
		0x15,       //Length of the remaining payload
		0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
		0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
		0x00, 0x00, // Major number
		0x00, 0x00, // Minor number
		0xC8        //2's complement of the Tx power (-56dB)
	};
	const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G'}; //???

	hci_le_set_scan_resp_data(0,NULL);
	aci_gap_set_discoverable(ADV_IND, 160, 160, PUBLIC_ADDR, NO_WHITE_LIST_USE, sizeof(local_name), local_name, 0, NULL, 0, 0);
	hci_le_set_advertising_data(sizeof(adv_data), adv_data);
}

void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle) {
	connection_handle = handle;
}

void GAP_DisconnectionComplete_CB(void) {
	connection_handle = 0;
	make_discoverable();
}

void Read_Request_CB(uint16_t handle) {
	if (connection_handle) {
		aci_gatt_allow_read(connection_handle);
	}
}

void HCI_Event_CB(void *pckt)
{
	hci_uart_pckt *hci_pckt = (hci_uart_pckt *) pckt;
	hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

	if (hci_pckt->type != HCI_EVENT_PKT) return;

	switch (event_pckt->evt) {
	case EVT_DISCONN_COMPLETE:
		GAP_DisconnectionComplete_CB();
		break;
	case EVT_LE_META_EVENT: {
			evt_le_meta_event *evt = (evt_le_meta_event *)event_pckt->data;
			switch (evt->subevent) {
			case EVT_LE_CONN_COMPLETE: {
					evt_le_connection_complete *cc = (evt_le_connection_complete *)evt->data;
					GAP_ConnectionComplete_CB(cc->peer_bdaddr,cc->handle);
				}
				break;
			}
		}
		break;
	case EVT_VENDOR: {
			evt_blue_aci *blue_evt = (evt_blue_aci *)event_pckt->data;
			switch (blue_evt->ecode) {
			case EVT_BLUE_GATT_READ_PERMIT_REQ: {
					evt_gatt_read_permit_req *pr = (evt_gatt_read_permit_req *)blue_evt->data;
					Read_Request_CB(pr->attr_handle);
				}
				break;
			case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
				break;
			}
			break;
		}
	}
}

void setup_gatt_db(void)
{
	uint16_t uuid;
	uuid = 0x180F;
	aci_gatt_add_serv(UUID_TYPE_16,  (uint8_t *) &uuid, PRIMARY_SERVICE, 10, &battery_svc_handle);
	uuid = 0x2A19;
	aci_gatt_add_char(battery_svc_handle, UUID_TYPE_16, (uint8_t *) &uuid, 1,
			CHAR_PROP_NOTIFY | CHAR_PROP_READ, ATTR_PERMISSION_NONE, GATT_DONT_NOTIFY_EVENTS,
			16, 0, &battery_char_handle);
}

int main(void)
{
	const char *name = "BlueNRG";

	PLL::init();
	SYSCLK::init();
	SYSTICK::init();
	SYSTICK::claim();
	BOARD::init();
	LED1::set_low();
	LED2::set_low();
	P3::init();
	BLUENRG_IRQ::init();
	BLUENRG_RESET::init();
	BLUENRG_CS::init();
	BLUENRG_CS::set_high();
	EXT_INTERRUPT::init();
	SPI::init();

	HCI_Init();
	BlueNRG_RST();
	aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
	aci_gatt_init();
        aci_gap_init(GAP_PERIPHERAL_ROLE, &service_handle, &dev_name_char_handle, &appearance_char_handle);
    	aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, strlen(name), (uint8_t *)name);
	aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED, OOB_AUTH_DATA_ABSENT, NULL, 7, 16, USE_FIXED_PIN_FOR_PAIRING, 123456, BONDING);
	setup_gatt_db();
	aci_hal_set_tx_power_level(1, 4);
	make_discoverable();
	while (1) {
		LED1::toggle();
		HCI_Process();
		aci_gatt_update_char_value(battery_svc_handle, battery_char_handle, 0, 1, &battery_level);
		battery_level++;
		if (battery_level > 100) battery_level = 50;
		TIMEOUT::set_and_wait(500);
	}
}
