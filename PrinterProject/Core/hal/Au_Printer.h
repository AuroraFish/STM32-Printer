#ifndef __AU_PRINTER_H__
#define __AU_PRINTER_H__

#include "Au_Config.h"
#include "Au_Motor.h"
#include "Au_Spi.h"
#include "Au_Ble.h"
#include "Au_Queue.h"


//每行总点数
#define TPH_DOTS_PER_LINE 384
//每行字节长度
#define TPH_DI_LEN 48
//所有通道打印
#define ALL_STB_NUM 0xff

//队列缓冲区打印
void Start_Printing_By_QueueBuffer(void);
//打印密度设置
void Set_Heat_Density(uint8_t Density);
//单通道打印
void Start_Printing_By_OneSTB(uint8_t STBnum, uint8_t* Data, uint32_t Len);
//打印机初始化
void Init_Printer(void);
//测试
void TestSTB(void);

#endif
