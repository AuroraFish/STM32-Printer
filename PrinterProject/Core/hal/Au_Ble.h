#ifndef __AU_BLE_H__
#define __AU_BLE_H__

#include "Au_Config.h"
#include "Au_Device.h"
#include "Au_Hal.h"
#include "Au_Queue.h"

void Init_Ble(void);

void Clean_Blepack_Count(void);

uint32_t Get_Blepack_Count(void);

void Ble_Status_Data_Clean(void);

void uart_cmd_handle(uint8_t Data);

bool Get_Ble_Connect(void);

void Ble_Report(void);

#endif
