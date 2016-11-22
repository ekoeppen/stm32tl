#include <clocks.h>
#include <gpio.h>
#include <tasks.h>
#include <exti.h>
#include <string.h>

#include <usb/usb.h>

#include <board.h>

typedef SYSCLK_T<> SYSCLK;
typedef BOARD_T::BOARD<BOARD_T::LED1 | BOARD_T::LED2 | BOARD_T::BUTTON> BOARD;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}


#if (defined STM32F1xx) || (defined STM32F4xx)
void EXTI0_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0>();
	exit_idle();
}

void EXTI1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<1>();
	exit_idle();
}

void EXTI2_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2>();
	exit_idle();
}

void EXTI3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<3>();
	exit_idle();
}

void EXTI4_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4>();
	exit_idle();
}

void EXTI9_5_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<5, 9>();
	exit_idle();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
}
#else
void EXTI0_1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0, 1>();
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

#endif

}

namespace USB_T {

struct CDC_CORE {
	static constexpr uint8_t device_descriptor[18] = {
		0x12, 0x01, 0x10, 0x01, 0x02, 0x00, 0x00, 0x40, 0xD8, 0x04, 0x0A, 0x00, 0x00, 0x01, 0x01, 0x02,
		0x03, 0x01
	};
	static constexpr uint8_t config_descriptor[63] = {
		0x09, 0x02, 0x3E, 0x00, 0x02, 0x01, 0x00, 0xC0, 0x80,
		0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x00,
		0x05, 0x24, 0x01, 0x00, 0x01,
		0x04, 0x24, 0x02, 0x06,
		0x05, 0x24, 0x06, 0x00, 0x01,
		0x07, 0x05, 0x82, 0x03, 0x40, 0x00, 0xFF,
		0x09, 0x04, 0x01, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00,
		0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x00,
		0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00, 0x00,
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

#ifdef STM32F0xx
	static constexpr uint8_t *u_id = (uint8_t *) 0x1ffff7ac;
#else
	static constexpr uint8_t *u_id = (uint8_t *) 0x1ffff7e8;
#endif
	static constexpr uint8_t line_coding[7] = {0x00, 0xc2, 0x01, 0x00, 0x00, 0x00, 0x08};

	static void get_serial_number_string_descriptor(const uint8_t **descriptor, uint32_t *length) {
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

	static bool get_descriptor(uint8_t type, uint8_t index, const uint8_t **descriptor, uint32_t *length)
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

	static bool handle_request(SETUP_PACKET *packet, const uint8_t **response_data, uint32_t *length) {
		bool r = true;
		switch (packet->bRequest) {
		case 0x20:
		case 0x22: *length = 0; break;
		case 0x21: *response_data = line_coding; *length = 7; break;
		default: r = false;
		}
		return r;
	}
};

constexpr uint8_t CDC_CORE::device_descriptor[18];
constexpr uint8_t CDC_CORE::config_descriptor[63];
constexpr uint8_t CDC_CORE::manufacturer_string_descriptor[14];
constexpr uint8_t CDC_CORE::product_string_descriptor[32];
constexpr uint8_t CDC_CORE::languages_descriptor[4];
constexpr uint8_t CDC_CORE::line_coding[7];

typedef CORE<CDC_CORE::get_descriptor, CDC_CORE::handle_request> USB_DRIVER;

}

extern "C" {

void OTG_FS_IRQHandler(void) {
	USB_T::USB_DRIVER::handle_irq();
}

}

typedef GPIO_T<PC, 9, MODE_ALTFUNC, TYPE_PUSH_PULL, SPEED_LOW, PULL_NONE, AF0> MCO2;

int main(void)
{
	BOARD::init();
	EXT_INTERRUPT::init();
	SYSTICK::init();
	// PORT_C::init();
	// MCO2::init();
	// RCC->CFGR |= RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_1;
	RCC->PLLCFGR = (96 << 6) | 16 | RCC_PLLCFGR_PLLQ_1;
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));
	RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
	USB_T::USB_DRIVER::init();
	while (1) {
		BUTTON::clear_irq();
		TIMEOUT::set(1000);
		if (BUTTON::template wait_for_irq<TIMEOUT>()) {
			LED1::set_high();
		} else {
			LED2::set_high();
		}
		SYSTICK::set_and_wait(100);
		LED1::set_low();
		LED2::set_low();
	}
	return 0;
}
