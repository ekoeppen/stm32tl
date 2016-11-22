/* Linker script to configure memory regions. */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08002000, LENGTH = 112K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
  PERIPH (rw) : ORIGIN = 0x40000000, LENGTH = 0x10000000
}

INCLUDE stm32f103_common.ld
