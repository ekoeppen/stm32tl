ifeq ($(BOOTLOADER),y)
	BL = _bootloader
endif

STM32F030F4_FAMILY := f0
STM32F030F4_LD := stm32f030f4.ld
STM32F030F4_CMSIS_DEVICE := STM32F0xx
STM32F030F4_H := stm32f0Xx.h
STM32F030F4_CPU_FLAGS := -mcpu=cortex-m0 -mthumb -DSTM32F030x6

STM32F030F6_FAMILY := f0
STM32F030F6_LD := stm32f030f6$(BL).ld
STM32F030F6_CMSIS_DEVICE := STM32F0xx
STM32F030F6_H := stm32f0xx.h
STM32F030F6_CPU_FLAGS := -mcpu=cortex-m0 -mthumb -DSTM32F030x6

STM32F072C8_FAMILY := f0
STM32F072C8_LD := stm32f072c8$(BL).ld
STM32F072C8_CMSIS_DEVICE := STM32F0xx
STM32F072C8_H := stm32f0xx.h
STM32F072C8_CPU_FLAGS := -mcpu=cortex-m0 -mthumb -DSTM32F072xB

STM32F103C8_FAMILY := f1
STM32F103C8_LD := stm32f103c8$(BL).ld
STM32F103C8_CMSIS_DEVICE := STM32F1xx
STM32F103C8_H := stm32f103x6.h
STM32F103C8_CPU_FLAGS := -mcpu=cortex-m3 -mthumb -DSTM32F103xB

STM32F103C8_FAMILY := f1
STM32F103C8_LD := stm32f103c8$(BL).ld
STM32F103C8_CMSIS_DEVICE := STM32F1xx
STM32F103C8_H := stm32f103x6.h
STM32F103C8_CPU_FLAGS := -mcpu=cortex-m3 -mthumb -DSTM32F103xB

STM32F103RB_FAMILY := f1
STM32F103RB_LD := stm32f103rb$(BL).ld
STM32F103RB_CMSIS_DEVICE := STM32F1xx
STM32F103RB_H := stm32f103xb.h
STM32F103RB_CPU_FLAGS := -mcpu=cortex-m3 -mthumb -DSTM32F103xB

STM32F407ZG_FAMILY := f4
STM32F407ZG_LD := stm32f40xxg$(BL).ld
STM32F407ZG_CMSIS_DEVICE := STM32F4xx
STM32F407ZG_H := stm32f4xx.h
STM32F407ZG_CPU_FLAGS := -mcpu=cortex-m4 -mthumb -march=armv7e-m -mfpu=fpv4-sp-d16 -DSTM32F40_41xxx

FAMILY := $($(CPU)_FAMILY)
LINKER_SCRIPT := $($(CPU)_LD)
CMSIS_DEVICE := $($(CPU)_CMSIS_DEVICE)
CMSIS_HEADER := $($(CPU)_H)
CPU_FLAGS := $($(CPU)_CPU_FLAGS)

ifndef FAMILY
$(error Unknown CPU $CPU)
endif

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-gcc
AS = arm-none-eabi-as
NM = arm-none-eabi-nm
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

ifdef BOARD
BOARD_INCLUDE := -DBOARD_ID_$(BOARD) -DBOARD_INCLUDE="<boards/$(BOARD).h>"
endif

OPTIMIZE ?= -Os

CORE_CFLAGS += -I. -I$(STM32TL) -I$(STM32TL)/common -I$(STM32TL)/$(FAMILY) -I$(STM32CMSIS)/Include -I$(STM32CMSIS)/Device/ST/$(CMSIS_DEVICE)/Include -fno-common $(OPTIMIZE) -g -Wall $(CPU_FLAGS) -ffunction-sections -fdata-sections -D$(CPU) -D$(CMSIS_DEVICE) $(BOARD_INCLUDE) $(PROJECT_FLAGS)
CFLAGS = $(CORE_CFLAGS) -std=c11 -MMD -c
CXXFLAGS = $(CORE_CFLAGS) -std=c++11 -MMD -c -fno-unwind-tables -fno-exceptions
LDFLAGS = -fdata-sections -ffunction-sections -L$(STM32TL)/$(FAMILY) --specs=nano.specs -Wl,--gc-sections -Wl,-T$(STM32TL)/$(FAMILY)/$(LINKER_SCRIPT) $(CORE_CFLAGS) -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys
ASFLAGS = $(CPU_FLAGS) -g
CPFLAGS = -Obinary
ODFLAGS	= -S

VPATH += .:$(STM32TL)/$(FAMILY):$(STM32TL)/common

ifdef BOARD
BUILD = $(BOARD)_$(CPU)
else
BUILD = $(CPU)
endif

C_SRCS += $(STM32TL)/$(FAMILY)/vectors.c
CXX_SRCS += $(STM32TL)/common/utils.cpp

# Generic targets
.PHONY: clean all

all: $(BUILD)/main.elf $(BUILD)/main.bin $(BUILD)/main.map $(BUILD)/main.lst

clean:
	rm -rf $(BUILD)

CXX_OBJS = $(addprefix $(BUILD)/, $(notdir $(CXX_SRCS:.cpp=.o)))
C_OBJS = $(addprefix $(BUILD)/, $(notdir $(C_SRCS:.c=.o)))
ASM_OBJS = $(addprefix $(BUILD)/, $(notdir $(ASM_SRCS:.S=.o)))
OBJS = $(CXX_OBJS) $(C_OBJS) $(ASM_OBJS)

DEP = $(OBJS:%.o=%.d)

-include $(DEP)

$(BUILD)/%.map : $(BUILD)/%.elf
	$(NM) $< | sort > $@

$(BUILD)/%.lst : $(BUILD)/%.elf
	$(OD) $(ODFLAGS) $< > $@
	$(SIZE) $<

$(BUILD)/%.bin : $(BUILD)/%.elf
	$(CP) $(CPFLAGS) $< $@

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

$(OBJS): | $(BUILD)

$(BUILD)/main.elf: $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS)

