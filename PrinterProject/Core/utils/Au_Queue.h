#ifndef __AU_QUEUE_H__
#define __AU_QUEUE_H__

#include "Au_Config.h"

//一行最大Byte
#define MAX_ONELINE_BYTE 48
//最大行数
#define MAX_LINE 275

//一行数据
typedef struct {
    uint8_t buffer[MAX_ONELINE_BYTE];
}ble_rx_buffer_t;

//Ble数据包
typedef struct {
    ble_rx_buffer_t Printer_Buffer[MAX_LINE];
    uint32_t r_index;
    uint32_t w_index;
    uint32_t left_line;
}ble_rx_t;

void Write_To_PrintBuffer(uint8_t * pData, size_t Length);

uint8_t * Read_To_PrintBuffer(void);

uint32_t Get_ble_rx_leftline(void);

void Clean_PrintBuffer(void);

void Init_Queue(void);

#endif
