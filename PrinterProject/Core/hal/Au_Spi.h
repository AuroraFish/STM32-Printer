#ifndef __AU_SPI_H__
#define __AU_SPI_H__

#include "Au_Config.h"
#include <SPI.H>

void Spi_Command(uint8_t* data_buffer, uint8_t data_len);

void Init_Spi(void);

#endif
