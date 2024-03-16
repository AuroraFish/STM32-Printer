#include "Au_Task.h"
#include "Au_Button.h"
#include "Au_Motor.h"
#include "Au_Printer.h"
#include "Au_Adc.h"

Button_T Button;
bool PrinterTest = false;
//长按结束标志
bool Is_Long_Click_Start = false;

/*
  * @brief  读取引脚电平
  * @param  pin:引脚
  * @return none	
  * @note   供按键模组使用的读取引脚电平函数
 */
static int Digital_Read(int pin)
{
    return HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);
}

/*
  * @brief  按键单击回调函数
  * @param  none
  * @return none	
  * @note   none
 */
void Btn_Down_Callback(void * Btn)
{
    PRINT_INFO("Button Down!");
    PrinterTest = true;
}

/*
  * @brief  按键双击回调函数
  * @param  none
  * @return none	
  * @note   未处理事务
 */
void Btn_Double_Callback(void * Btn)
{
    PRINT_INFO("Button Double Click");
}

/*
  * @brief  按键长按回调函数
  * @param  none
  * @return none	
  * @note   none
 */
void Btn_Long_Callback(void * Btn)
{
    if (Is_Long_Click_Start == true) {
        return;
    }
    Is_Long_Click_Start = true;
    PRINT_INFO("Button Long");
    device_state_t * pDevice = Get_Device_State();
    bool Need_Led = false;
    
    //不缺纸执行
    if (pDevice->Paper_State == PAPER_STATUS_NORMAL) {
        if (pDevice->Printer_State == PRINTER_STATUS_INIT || pDevice->Printer_State == PRINTER_STATUS_FINISH) {
            PRINT_INFO("Btn_Long_Callback: PaperStartMove");
            Motor_Start();
        }
        else {
            Need_Led = true;
        }
    }
    else {
        Need_Led = true;
    }
    if (Need_Led) {
        //Run_Beep(BEEP_WARN); 未使用
        Run_Led(LED_WARN);
    }
}

/*
  * @brief  长按释放回调函数
  * @param  none
  * @return none	
  * @note   none
 */
void Btn_Long_Free_Callback(void * Btn)
{
    Is_Long_Click_Start = false;
    PRINT_INFO("Btn_Long_Free_Callback: StopPaperMove");
    Motor_Stop();
}

/*
  * @brief  供按键模组使用的读取引脚电平函数
  * @param  none
  * @return none	
  * @note   none
 */
uint8_t Read_Button_Level(void)
{
    return Digital_Read(PIN_KEY);
}

/*
  * @brief  按键初始化
  * @param  none
  * @return none	
  * @note   none
 */
void Init_Button(void)
{
    //按键名 按键句柄 电平函数 触发沿
    Button_Create("Button", &Button, Read_Button_Level, LOW);
    Button_Attach(&Button, BUTTON_DOWN, Btn_Down_Callback);
    Button_Attach(&Button, BUTTON_DOUBLE, Btn_Double_Callback);
    Button_Attach(&Button, BUTTON_LONG, Btn_Long_Callback);
    Button_Attach(&Button, BUTTON_LONG_FREE, Btn_Long_Free_Callback);
}

/*
  * @brief  处理上报事件
  * @param  none
  * @return none	
  * @note   none
 */
void Run_Report(void)
{
    //上报状态定时器定时时间已到, 10s一次
    if (Get_State_Timeout()) {
        Clean_State_Timeout();
        //读取设备状态电量,纸张,温度
        Read_All_Hal();
        
        if (Get_Ble_Connect()) {
            PRINT_INFO("Run_Report: Report Device State");
            Ble_Report();
        }
    }
    
    if (Read_Paper_Irq_Need_Report_Status()) {
        PRINT_INFO("Run_Report: Paper irq, Need Report");
        Ble_Report();
    }
}

/*
  * @brief  处理打印相关事件
  * @param  none
  * @return none	
  * @note   none
 */
void Run_Printer(void)
{
    device_state_t * pDevice = Get_Device_State();
    
    #ifdef START_PRINTER_WHEN_FINISH_RAED
    
        if (pDevice->Read_Ble_Finish == true) {
            if (pDevice->Printer_State == PRINTER_STATUS_INIT || pDevice->Printer_State == PRINTER_STATUS_FINISH) {
                pDevice->Read_Ble_Finish = false;
                pDevice->Printer_State = PRINTER_STATUS_START;
                Ble_Report();
                PRINT_INFO("Run_Printer:Printer Start");
                Run_Led(LED_PRINTER_START);
            }
        }
    #else
        // 接收大于100条时, 触发打印
        if (Get_ble_rx_leftline() > 100) {
            if (pDevice->Printer_State == PRINTER_STATUS_INIT || pDevice->Printer_State == PRINTER_STATUS_FINISH) {
                pDevice->Printer_State = PRINTER_STATUS_START;
                Ble_Report();
                PRINT_INFO("Run_Printer:Printer Start");
                Run_Led(LED_PRINTER_START);
            }
        }
    #endif
        
    //开始打印
    if (pDevice->Printer_State == PRINTER_STATUS_START) {
        //正常打印
        Start_Printing_By_QueueBuffer();
        pDevice->Printer_State = PRINTER_STATUS_FINISH;
    }
}

/*
  * @brief  上报事件任务
  * @param  none
  * @return none	
  * @note   none
 */
void Task_Report(void * arg) 
{
    int Count = 0;
    PRINT_INFO("Task Report Init");
    
    while (1) {
        Run_Report();
        vTaskDelay(100);
        Count++;
        
        if (Count >= 50) {
            Count = 0;
            PRINT_INFO("Task Report Info");
        }
    } 
}

/*
  * @brief  按键驱动任务
  * @param  none
  * @return none	
  * @note   none
 */
void Task_Button(void * arg) {
    int Count = 0;
    PRINT_INFO("Task Button Init");
    
    while (1) {
        Button_Handle();
        vTaskDelay(20);
        Count++;
        
        if (Count >= 250) {
            Count = 0;
            PRINT_INFO("Task Button Info");
        }
    } 
}

/*
  * @brief  打印事件任务
  * @param  none
  * @return none	
  * @note   none
 */
void Task_Printer(void * arg) 
{
    int Count = 0;
    Init_Ble();
    PRINT_INFO("Task Printer Init");
    
    while (1) {
        Ble_Status_Data_Clean();
        Run_Printer();
        
        vTaskDelay(1);
        
        if (PrinterTest) {
            PrinterTest = false;
            TestSTB();
        }
        
        Count++;
        if (Count >= 5000) {
            Count = 0;
            PRINT_INFO("Task Printer Info");
        }
    }
}

void Init_Task(void)
{
    PRINT_INFO("Init Task");
    
    Init_Device_State();
    Init_Timer();
    Init_Hal();
    Init_Queue();
    Init_Adc();
    Init_Button();
    Init_Printer();
    
    /* 1.任务函数 2.任务名 3.任务栈 4.任务参数 5.任务优先级 6.任务句柄 */
    xTaskCreate(Task_Report, "TaskReport", 128, NULL, 1, NULL);
    
    xTaskCreate(Task_Button, "TaskButton", 128, NULL, 0, NULL);
    
    xTaskCreate(Task_Printer, "TaskPrinter", 256, NULL, 2, NULL);
}
