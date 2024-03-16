#ifndef __AU_PRINTER_H__
#define __AU_PRINTER_H__

#include "Au_Config.h"
#include "Au_Motor.h"
#include "Au_Spi.h"
#include "Debug.h"

//每行总点数
#define TPH_DOTS_PER_LINE 384
//每行字节长度
#define TPH_DI_LEN 48
//所有通道打印
#define ALL_STB_NUM 0xff

void Set_Heat_Density(uint8_t Density);
void Start_Printing_By_OneSTB(uint8_t STBnum, uint8_t* Data, uint32_t Len);
void Init_Printer(void);
void TestSTB(void);

#endif
