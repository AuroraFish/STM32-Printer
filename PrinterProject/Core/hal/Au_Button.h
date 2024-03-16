#ifndef __AU_BUTTON_H__
#define __AU_BUTTON_H__

#include <stdio.h>
#include "Debug.h"

/*
    
*/


// 按键名最大长度 
#define Button_Name_Length 32  

#define Singled_And_Double_Trigger      1   // 是否支持单击/双击同时存在触发
#define Long_Free_Trigger               1   // 是否支持长按释放才触发

#define Button_Debounce_Time            2   // 消抖时间         (n-1)*调用周期   
#define Button_Long_Cycle               1   // 长按触发周期时间 (n-1)*调用周期  
#define Button_Double_Time              15  // 双击间隔时间     (n-1)*调用周期   
#define Button_Long_Time                50  // 持续 (n-1)*调用周期 ms 认为是长按事件

void function(void);

// 触发回调的事件
typedef enum {          
    BUTTON_DOWN = 0,
    BUTTON_UP,
    BUTTON_DOUBLE,
    BUTTON_LONG,
    BUTTON_LONG_FREE,
    BUTTON_ALL_TRIGGER,
    NUMBER_OF_EVENT,
    NONE_TRIGGER
} Button_Event; 

#define TRIGGER_CB(event)   \
        if(Btn->Cb_Function[event]) \
            Btn->Cb_Function[event]((Button_T*)Btn)

// 按键触发回调函数，需要实现
typedef void (*Button_Callback)(void*);

// 按键对应结构体变量
typedef struct Button {
    uint8_t (*Read_Button_Pin)(void);       // 读取按键电平函数，需要实现
    
    char Button_Name[Button_Name_Length];   
    
    uint8_t Button_State;                   // 按键当前状态
    uint8_t Button_Last_State;              // 按键上次状态, 用以判断双击
    uint8_t Button_Trigger_Level;           // 按键触发电平
    uint8_t Button_Last_Level;              // 按键当前电平
    
    uint8_t Button_Trigger_Event;           // 按键触发事件, 用以回调函数
    
    Button_Callback Cb_Function[NUMBER_OF_EVENT];
    
    uint8_t Button_Cycle;                   // 连续长按时间
    uint8_t Timer_Count;                    // 双击之间时间计算
    uint8_t Debounce_Time;                  // 消抖时间
    uint8_t Long_Time;                      // 按键按下持续时间
    
    struct Button * Next;
}Button_T;

// 供外部调用的函数声明
void Button_Create(const char* Name, Button_T* Btn, uint8_t(*Read_Btn_Pin)(void), uint8_t Btn_Trigger_Level);
void Button_Attach(Button_T* Btn, Button_Event Btn_Event, Button_Callback Btn_Cb);

void Search_Button(void);
void Button_Delete(Button_T* Btn);

void Get_Button_EventInfo(Button_T* Btn);
uint8_t Get_Button_Event(Button_T* Btn);
uint8_t Get_Button_State(Button_T* Btn);

void Button_Single_Handle(Button_T* Btn);
void Button_Handle(void);

#endif
