PRG ?= main
CPU ?= STM32F072C8
BOARD ?= lqfp48_usb_s
STM32TL ?= ../..
STM32CMSIS ?= ../../../CMSIS

BLUENRG ?= ../../../../BlueNRG/Projects/Middlewares/ST/STM32_BlueNRG/SimpleBlueNRG_HCI

CXX_SRCS = main.cpp
C_SRCS = $(BLUENRG)/hci/hci.c \
	 $(BLUENRG)/controller/bluenrg_gap_aci.c \
	 $(BLUENRG)/controller/bluenrg_gatt_aci.c \
	 $(BLUENRG)/controller/bluenrg_hal_aci.c \
	 $(BLUENRG)/controller/bluenrg_l2cap_aci.c \
	 $(BLUENRG)/controller/bluenrg_utils.c \
	 $(BLUENRG)/utils/gp_timer.c \
	 $(BLUENRG)/utils/list.c \
	 $(BLUENRG)/utils/osal.c

# OPTIMIZE = -O0

include $(STM32TL)/common/common.mk

CORE_CFLAGS += -I$(BLUENRG)/includes -Wno-attributes -DBLUENRG_CONFIG=BLUENRG_16_MHZ
VPATH += :$(BLUENRG)/hci:$(BLUENRG)/hci/controller:$(BLUENRG)/utils
