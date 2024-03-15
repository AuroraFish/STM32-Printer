#ifndef _EM_CONFIG_H_
#define _EM_CONFIG_H_

#include "stm32f1xx_hal.h"

#include "FreeRTOS.h"
#include "cmsis_os.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define _FREERTOS_OPEN_ 

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"         // 自定义打印服务UUID

#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"  // 自定义打印特征UUID

#define BLE_NAME "Mini-Printer"

//接收完成所有数据才开始打印
#define START_PRINTER_WHEN_FINISH_RAED 1

//按键引脚
//34 35 36 39仅可以作为输入INPUT,不支持INPUT_PULLUP
#define PIN_KEY 5
//缺纸检测引脚
#define PIN_PAPER 35
//电量相关引脚
#define PIN_BATTERY_ADC 34
#define PIN_ADC_EN 4
//set the resolution to 12 bits (0-4096)
#define BATTERY_ADC_BIT 12
//热敏电阻
#define PIN_TEMPERATRUE 36
//蜂鸣器
#define PIN_BEEP 18
#define PIN_LED 18

//V2 电机引脚
// #define PIN_MOTOR_AP 19
// #define PIN_MOTOR_AM 21
// #define PIN_MOTOR_BP 22
// #define PIN_MOTOR_BM 23
//V3 电机引脚
#define PIN_MOTOR_AP 23
#define PIN_MOTOR_AM 22
#define PIN_MOTOR_BP 21
#define PIN_MOTOR_BM 19

//打印头数据引脚
#define PIN_LAT 12
#define PIN_SCK 2
#define PIN_SDA 15

//拆机 V2
// #define PIN_STB1 32
// #define PIN_STB2 33
// #define PIN_STB3 25
// #define PIN_STB4 26
// #define PIN_STB5 27
// #define PIN_STB6 14

#define PIN_STB1 14
#define PIN_STB2 27
#define PIN_STB3 26
#define PIN_STB4 25
#define PIN_STB5 33
#define PIN_STB6 32

//原厂 V3
//#define PIN_STB1 26
//#define PIN_STB2 27
//#define PIN_STB3 14
//#define PIN_STB4 32
//#define PIN_STB5 33
//#define PIN_STB6 25

//拆机 V2
// #define PIN_STB1 14
// #define PIN_STB2 27
// #define PIN_STB3 26
// #define PIN_STB4 25
// #define PIN_STB5 33
// #define PIN_STB6 32
//打印头电源升压控制引脚
#define PIN_VHEN 17

#ifdef _FREERTOS_OPEN_
// #define MOTOR_WATI_TIME 4   //ms
// #define PRINT_TIME 2        //ms
// #define PRINT_END_TIME 2  //ms
#define PRINT_TIME 2600         //打印加热时间
#define PRINT_END_TIME 200      //冷却时间
#define MOTOR_WAIT_TIME 4000    //电机一步时间
#define LAT_TIME 1              //数据锁存时间
#else
//打印头电机参数
#define PRINT_TIME 1700         //打印加热时间
#define PRINT_END_TIME 200      //冷却时间
#define MOTOR_WAIT_TIME 4000    //电机一步时间
#define LAT_TIME 1              //数据锁存时间
#endif


#include "tim.h"
#define DLY_TIM_Handle (&htim1)  // Timer handle
#define hal_delay_us(nus) do { \
    __HAL_TIM_SET_COUNTER(DLY_TIM_Handle, 0); \
    __HAL_TIM_ENABLE(DLY_TIM_Handle); \
    while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle) < (nus)); \
    __HAL_TIM_DISABLE(DLY_TIM_Handle); \
} while(0)


#ifdef _FREERTOS_OPEN_
#define us_delay(ms) hal_delay_us(ms)
//#define us_delay(ms) delayMicroseconds(ms)
#else
#define us_delay(ms) delayMicroseconds(ms)
#endif

#define LOW               0x0
#define HIGH              0x1

#endif
