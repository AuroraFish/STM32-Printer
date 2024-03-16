#ifndef __AU_MOTOR_H__
#define __AU_MOTOR_H__

#include "Au_Config.h"
#include "Debug.h"
#include <stdio.h>

void Init_Motor(void);
void Motor_Run(void);
void Motor_Run_Steps(uint32_t steps);
void Motor_Start(void);
void Motor_Stop(void);
uint8_t Get_Phase(void);

#endif
