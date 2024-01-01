#include "Au_Button.h"
#include <string.h>

static struct Button* Button_Head = NULL;

static char* Str_Copy(char* des,const char* source);
static void Add_Button(Button_T* Btn);
static void Print_Button_Info(Button_T* Btn);

/*
  * @brief  按键创建
  * @param  Name:按键名, Btn:按键句柄, 
  * @param  Read_Btn_Pin:电平读取函数
  * @param	Btn_Trigger_Level:按键触发电平
  * @return none	
  * @note   none
 */
void Button_Create(const char* Name, Button_T* Btn, uint8_t(*Read_Btn_Pin)(void), uint8_t Btn_Trigger_Level) 
{
	if (Btn == NULL) {
		PRINT_ERR("struct button is null!");
		ASSERT(ASSERT_ERR);
	}
	
	memset(Btn, 0, sizeof(struct Button));
	Str_Copy(Btn->Button_Name, Name);
	
	Btn->Button_State = NONE_TRIGGER;
	Btn->Button_Last_State = NONE_TRIGGER;
	Btn->Button_Trigger_Event = NONE_TRIGGER;
	Btn->Read_Button_Pin = Read_Btn_Pin;
	Btn->Button_Trigger_Level = Btn_Trigger_Level;
	Btn->Button_Last_Level = Btn->Read_Button_Pin();
	Btn->Debounce_Time = 0;
	
	PRINT_DEBUG("Button create success!");
	Add_Button(Btn);
	Print_Button_Info(Btn);
}

/*
  * @brief  为按键绑定回调函数
  * @param  Btn:按键, Btn_Event:按键触发事件, Btn_Cb:回调
  * @return none	
  * @note   none
 */
void Button_Attach(Button_T* Btn, Button_Event Btn_Event, Button_Callback Btn_Cb)
{
	if (Btn == NULL) {
		PRINT_ERR("struct button is NULL!");
	}
	
	if (BUTTON_ALL_TRIGGER == Btn_Event) {
		for (uint8_t i = 0; i < NUMBER_OF_EVENT; i++) 
			Btn->Cb_Function[i] = Btn_Cb;
	}
	else {
		Btn->Cb_Function[Btn_Event] = Btn_Cb;
	}
}

/*
  * @brief  单个处理按键
  * @param  Btn:按键
  * @return none	
  * @note   周期调用, 20-50ms
 */
void Button_Single_Handle(Button_T* Btn)
{
    // 获取当前电平
    uint8_t Cur_Level = (uint8_t)Btn->Read_Button_Pin();
    
    // 电平发生变化, 及消抖
    if ((Cur_Level != Btn->Button_Last_Level) && (++(Btn->Debounce_Time) >= Button_Debounce_Time)) {
        Btn->Button_Last_Level = Cur_Level;
        Btn->Debounce_Time = 0;
        
        // 如果按键没被按下, 改变状态为按下
        if ((Btn->Button_State == NONE_TRIGGER) || (Btn->Button_State == BUTTON_DOUBLE)) {
            Btn->Button_State = BUTTON_DOWN;
        }
        else if (Btn->Button_State == BUTTON_DOWN) {
            Btn->Button_State = BUTTON_UP;
            TRIGGER_CB(BUTTON_UP);      /* 回调 */
            PRINT_DEBUG("释放按键");
        }
    }
    
    switch(Btn->Button_State) {
        // 按下状态
        case BUTTON_DOWN: {
            if (Btn->Button_Last_Level == Btn->Button_Trigger_Level) {
                Btn->Button_Trigger_Event = BUTTON_DOWN;
                
                // 如大于长按时间, 释放前更新为长按
                if (++(Btn->Long_Time) >= Button_Long_Time) {
                    #if Long_Free_Trigger
                    
                    Btn->Button_Trigger_Event = BUTTON_LONG;
                    
                    #else 
                    
                    // 连续触发长按的时间
                    if (++(Btn->Button_Cycle) >= Button_Long_Cycle) {
                        Btn->Button_Cycle = 0;
                        Btn->Button_Trigger_Event = BUTTON_LONG;
                        TRIGGER_CB(BUTTON_LONG);
                    }
                    
                    #endif
                    
                    if (Btn->Long_Time == 0xff) {
                        Btn->Long_Time = BUTTON_LONG;
                    }
                    PRINT_DEBUG("长按");
                }
                
            }
            break;
        }
        
        case BUTTON_UP: {
            // 此处判断单击和双击
            if (Btn->Button_Trigger_Event == BUTTON_DOWN) {
                // 双击
                if ((Btn->Timer_Count <= Button_Double_Time) && (Btn->Button_Last_State == BUTTON_DOUBLE)) {
                    Btn->Button_Trigger_Event = BUTTON_DOUBLE;
                    TRIGGER_CB(BUTTON_DOUBLE);
                    PRINT_DEBUG("双击");
                    Btn->Button_State = NONE_TRIGGER;
                    Btn->Button_Last_State = NONE_TRIGGER;
                }
                else {
                    // 单击
                    Btn->Timer_Count = 0;
                    Btn->Long_Time = 0;
                    
                    #if (Singled_And_Double_Trigger == 0)
                        TRIGGER_CB(BUTTON_DOWN);
                    #endif
                    
                    Btn->Button_State = BUTTON_DOUBLE;
                    Btn->Button_Last_State = BUTTON_DOUBLE;
                }
                
            }
            // 长按释放
            else if (Btn->Button_Trigger_Event == BUTTON_LONG) {
                #if Long_Free_Trigger
                    TRIGGER_CB(BUTTON_LONG_FREE);
                #endif
                
                Btn->Long_Time = 0;
                Btn->Button_State = NONE_TRIGGER;
                Btn->Button_Last_State = BUTTON_LONG;
            }
            
            break;
        }
        
        // 计算两次按下之间的时间
        case BUTTON_DOUBLE: {
            Btn->Timer_Count++;
            // 超过双击间隔时间, 状态置空
            if (Btn->Timer_Count >= Button_Double_Time) {
                Btn->Button_State = NONE_TRIGGER;
                Btn->Button_Last_State = NONE_TRIGGER;
            }
            
            #if Singled_And_Double_Trigger
                
                if ((Btn->Timer_Count >= Button_Double_Time) && (Btn->Button_Last_State != BUTTON_DOWN)) {
                    Btn->Timer_Count = 0;
                    TRIGGER_CB(BUTTON_DOWN);    
                    Btn->Button_State = NONE_TRIGGER;
                    Btn->Button_Last_State = BUTTON_DOWN;
                }
                
            #endif
                
            break;
        }
        
        default: 
            break;
    }
}

/*
  * @brief  遍历处理按键
  * @param  none
  * @return none	
  * @note   周期调用, 20-50ms
 */
void Button_Handle(void)
{
    struct Button* Btn;
    for (Btn = Button_Head; Btn != NULL; Btn = Btn->Next) {
        Button_Single_Handle(Btn);
    }
}

/*
  * @brief  获取按键状态
  * @param  Btn:按键
  * @return none	
  * @note   none
 */
uint8_t Get_Button_State(Button_T* Btn)
{
    return (uint8_t)(Btn->Button_State);
}

/*
  * @brief  获取按键触发的事件
  * @param  Btn:按键
  * @return none	
  * @note   none
 */
uint8_t Get_Button_Event(Button_T* Btn)
{
    return (uint8_t)(Btn->Button_Trigger_Event);
}

/*
  * @brief  获取按键触发事件
  * @param  Btn:按键
  * @return none	
  * @note   none
 */
void Get_Button_EventInfo(Button_T* Btn)
{
    for (uint8_t i = 0; i<NUMBER_OF_EVENT-1; i++) {
        if (Btn->Cb_Function[i] != 0) {
            PRINT_INFO("Button_Event:%d", i);
        }
    }
}

/*
  * @brief  删除已创建的按键
  * @param  Btn:按键
  * @return none	
  * @note   none
 */
void Button_Delete(Button_T* Btn)
{
	struct Button ** cur;
	for (cur = &Button_Head; *cur!=NULL; ) {
		struct Button* temp = *cur;
		
		if (temp == Btn) {
			*cur = temp->Next;
		}
		else {
			cur = &temp->Next;
		}
	}
}

/*
  * @brief  遍历按键
  * @param  none
  * @return none	
  * @note   none
 */
void Search_Button(void)
{
	struct Button* temp;
	for (temp = Button_Head; temp != NULL; temp = temp->Next) {
		PRINT_INFO("Button node have %s", temp->Button_Name);
	}
}

/*
  * @brief  打印按键信息
  * @param  Btn:按键
  * @return none	
  * @note   none
 */
static void Print_Button_Info(Button_T* Btn)
{
	PRINT_INFO("Button struct information:\r\n"
               "          Btn->Name: %s\r\n"
               "          Btn->Button_State: %d\r\n"
               "          Btn->Button_Trigger_Event: %d\r\n"
               "          Btn->Button_Trigger_Level: %d\r\n"
               "          Btn->Button_Last_Level:%d \r\n", 
               Btn->Button_Name,
               Btn->Button_State,
               Btn->Button_Trigger_Event,
               Btn->Button_Trigger_Level,
               Btn->Button_Last_Level);
	Search_Button();
}

/*
  * @brief  将所有按键链在一起
  * @param  Btn:按键
  * @return none	
  * @note   none
 */
static void Add_Button(Button_T* Btn)
{
	Btn->Next = Button_Head;
	Button_Head = Btn;
}

/*
  * @brief  字符串拷贝
  * @param  des:目标字符串, source:原字符串
  * @return 目标串的地址	
  * @note   none
 */
static char* Str_Copy(char* des,const char* source) 
{
	char* r = des;
	uint8_t count = 0;
	
	if ((des == NULL) || (source == NULL)) {
		PRINT_ERR("Button name is null!");
		ASSERT(ASSERT_ERR);
	}
	
	while ((*r++ = *source++) != '\0') {
		count++;
	}
	if (count >= Button_Name_Length) {
		PRINT_ERR("Button name too long!");
		ASSERT(ASSERT_ERR);
	}
	
	return des;
}



/*
  * @brief  串口重定向至printf
  * @param  ch:输出字符 , f:文件指针
  * @return ch:输出字符
  * @note   none
 */
//int fputc(int ch, FILE *f)
//{
//    uint8_t temp[1] = {ch};
//    HAL_UART_Transmit(&huart1, temp, 1, 2);
//    return ch;
//}


