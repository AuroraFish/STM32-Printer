#ifndef _EM_MOTOR_H_
#define _EM_MOTOR_H_

#include "Au_Config.h"

void motor_run_step(uint32_t steps);
void motor_start(void);
void motor_stop(void);
void init_motor(void);
void motor_run(void);

#endif

