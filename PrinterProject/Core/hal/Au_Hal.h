#ifndef __AU_HAL_H__
#define __AU_HAL_H__

#include "Au_Config.h"
#include "Au_Device.h"

//蜂鸣器状态(未使用)
typedef enum {
    BEEP_CONNECT = 0,
    BEEP_WARN,
    BEEP_PRINTER_START,
}beep_state_t;

typedef enum {
    LED_CONNECT = 0,
    LED_WARN,
    LED_PRINTER_START,
    LED_BLE_INIT,
}led_state_t;

void Run_Beep(beep_state_t Status);

void Run_Led(led_state_t Status);

void Read_All_Hal(void);

void Init_Hal(void);

bool Read_Paper_Irq_Need_Report_Status(void);

#endif
