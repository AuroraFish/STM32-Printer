#ifndef __AU_DEVICE_H__
#define __AU_DEVICE_H__

#include "Au_Config.h"

//打印机状态
typedef enum {
    PRINTER_STATUS_INIT = 0,
    PRINTER_STATUS_START,
    PRINTER_STATUS_WORKING,
    PRINTER_STATUS_FINISH,
}printer_state_t;

//纸张状态(正常/缺纸)
typedef enum {
    PAPER_STATUS_NORMAL = 0,
    PAPER_STATUS_LACK,
}paper_state_t;

//设备状态
typedef struct {
    bool            Read_Ble_Finish;
    uint8_t         Battery;
    uint8_t         Temperature;
    paper_state_t   Paper_State;
    printer_state_t Printer_State;
}device_state_t;

void Init_Device_State(void);

device_state_t * Get_Device_State(void);

void Set_Read_Ble_Finish(bool Finish);

void Set_Device_Paper_Status(paper_state_t Status);

#endif
