#pragma once

uint8_t SdkEvalSPI_Irq_Pin(void);
void Clear_SPI_EXTI_Flag();

void Enable_SPI_IRQ(void);
void Disable_SPI_IRQ(void);

void BlueNRG_RST(void);
void BlueNRG_HW_Bootloader(void);
uint8_t BlueNRG_SPI_Read_All(uint8_t *buffer, uint8_t buff_size);
int16_t BlueNRG_SPI_Write(uint8_t* data1, uint8_t* data2, uint16_t Nb_bytes1, uint16_t Nb_bytes2);
void BlueNRG_Power_Down(void);
void BlueNRG_Power_Up(void);
uint8_t BlueNRG_DataPresent(void);
void BlueNRG_IRQ_High(void);
void BlueNRG_Release_IRQ(void);
void BlueNRG_BootloaderPatch(void);
