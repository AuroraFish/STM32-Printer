#ifndef __TESTPRINTER_H__
#define __TESTPRINTER_H__

#include "TestMotor.h"
#include "Au_Config.h"
#include "Au_Spi.h"

//每行总点数
#define TPH_DOTS_PER_LINE 384
//每行字节长度 384/8
#define TPH_DI_LEN 48
//所有通道打印
#define ALL_STB_NUM 0xFF

void start_printing_by_onestb(uint8_t stbnum, uint8_t *data, uint32_t len);

void init_printer(void);

void testSTB(void);

#endif
