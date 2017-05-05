MODE ?= TX
PRG ?= main_$(MODE)
CPU ?= STM32F072C8
BOARD ?= lqfp48_usb_s
STM32TL ?= ../..
STM32CMSIS ?= ../../../CMSIS

PROJECT_FLAGS = -DMODE=$(MODE)

CXX_SRCS = main.cpp

OPTIMIZE = -O0

include $(STM32TL)/common/common.mk
