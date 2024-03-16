#ifndef __AU_TIMER_H__
#define __AU_TIMER_H__

#include "Au_Config.h"

void Init_Timer(void);

bool Get_State_Timeout(void);

void Clean_State_Timeout(void);

void Open_Printer_Timeout_Timer(void);

void Close_Printer_Timeout_Timer(void);

bool Get_Printer_Timeout_Status(void);

#endif
