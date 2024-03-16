#include "Au_Timer.h"
#include "Debug.h"

bool State_Timeout = false;
bool Printer_Timeout = false;

//上报设备状态定时器句柄
osTimerId myStateTimerHandle = false;
//打印超时定时器句柄
osTimerId Printer_TimeoutHandle = false;

/*
  * @brief  上报设备状态定时器回调
  * @param  arg
  * @return none	
  * @note   none
 */
void State_Timer_Callback(void const * arg)
{
    PRINT_DEBUG("State_Timer CB now");
    State_Timeout = true;
}

/*
  * @brief  Timer初始化
  * @param  none
  * @return none	
  * @note   初始化并启动上报设备状态定时器,间歇触发,定时10s
 */
void Init_Timer(void)
{
    osTimerDef(myStateTimer, State_Timer_Callback);
    myStateTimerHandle = osTimerCreate(osTimer(myStateTimer), osTimerPeriodic, NULL);
    osTimerStart(myStateTimerHandle, 10000);
}

/*
  * @brief  返回上报状态标志
  * @param  none
  * @return none	
  * @note   none
 */
bool Get_State_Timeout(void)
{
    return State_Timeout;
}

/*
  * @brief  清除上报状态标志
  * @param  none
  * @return none	
  * @note   none
 */
void Clean_State_Timeout(void)
{
    State_Timeout = false;
}

/*
  * @brief  打印超时回调函数
  * @param  arg
  * @return none	
  * @note   none
 */
void Printer_Timer_Callback(void const * arg)
{
    PRINT_DEBUG("Printer_Timer CB now");
    Printer_Timeout = true;
}

/*
  * @brief  启动打印超时定时器
  * @param  none
  * @return none	
  * @note   单次触发,定时20s
 */
void Open_Printer_Timeout_Timer(void)
{
    Printer_Timeout = false;
    osTimerDef(PrinterTimer, Printer_Timer_Callback);
    Printer_TimeoutHandle = osTimerCreate(osTimer(PrinterTimer), osTimerOnce, NULL);
    osTimerStart(Printer_TimeoutHandle, 20000);
}

/*
  * @brief  停止打印超市定时器
  * @param  none
  * @return none	
  * @note   none
 */
void Close_Printer_Timeout_Timer(void)
{
    osTimerDelete(myStateTimerHandle);
}

/*
  * @brief  获取打印超时标志
  * @param  none
  * @return none	
  * @note   none
 */
bool Get_Printer_Timeout_Status(void)
{
    return Printer_Timeout;
}
