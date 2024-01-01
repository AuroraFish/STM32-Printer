#ifndef __AU_CONFIG_H__
#define __AU_CONFIG_H__

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "Debug.h"

#define _FREERTOS_OPEN_

#define SET 0x1
#define RESET 0x0

//电机引脚
#define PIN_MOTOR_AP 23
#define PIN_MOTOR_AM 22
#define PIN_MOTOR_BP 21
#define PIN_MOTOR_BM 19

#define MOTOR_WAIT_TIME 4000	//电机一步时间

#include "tim.h"
#define DLY_TIM_HANDLE (&htim1)
#define hal_delay_us(nus) do { \
    __HAL_TIM_SET_COUNTER(DLY_TIM_HANDLE, 0); \
    __HAL_TIM_ENABLE(DLY_TIM_HANDLE); \
    while (__HAL_TIM_GET_COUNTER(DLY_TIM_HANDLE) < (nus)); \
    __HAL_TIM_DISABLE(DLY_TIM_HANDLE); \
} while(0)

#ifdef _FREERTOS_OPEN_
#define us_delay(ms) hal_delay_us(ms)
#else
#define us_delay(ms) delayMicroseconds(ms)
#endif

#endif
