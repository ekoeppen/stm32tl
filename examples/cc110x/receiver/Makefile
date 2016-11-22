PRG = receiver

ifeq ($(BOOTLOADER),y)
	BL = _bootloader
endif

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
LD = arm-none-eabi-ld
AS = arm-none-eabi-as
NM = arm-none-eabi-nm
SIZE = arm-none-eabi-size

INC = -I. -I.. -I../.. -I../../.. -I../../../..

CFLAGS = $(INC) -Os -g -mcpu=cortex-m3 -mthumb -nostartfiles -ffunction-sections -fdata-sections -DSTM32F10X_MD
CXXFLAGS = $(CFLAGS) -std=c++11 -fno-unwind-tables -fno-exceptions
LDFLAGS = -nostdlib --gc-sections -L../../.. -L../../../../libaeabi-cortexm0 -T../../../stm32f103xb$(BL).ld
LIBS = -laeabi-cortexm0

CPP_SRCS = main.cpp
C_SRCS = ../../../vectors.c ../../../utils.c

OBJS = $(CPP_SRCS:.cpp=.o) $(C_SRCS:.c=.o) $(ARM_SRCS:.s=.o)
HDRS = $(wildcard *.h)
INTERMEDIATES = $(PRG).lst $(PRG).bin $(PRG).elf $(PRG).map

all: $(OBJS)
	$(LD) -o $(PRG).elf $(LDFLAGS) $(OBJS) $(LIBS)
	$(OBJCOPY) -O binary $(PRG).elf $(PRG).bin
	$(OBJDUMP) -S $(PRG).elf > $(PRG).lst
	$(NM) $(PRG).elf | sort > $(PRG).map
	$(SIZE) $(PRG).elf

%.elf : %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $($*_FLAGS) $(COMMON_SRCS) $<
	$(OBJDUMP) -S $@ > $*.lst
	$(OBJCOPY) -O binary $@ $*.bin
	$(SIZE) $@

clean:
	rm -f $(OBJS) $(INTERMEDIATES)

