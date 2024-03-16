#include "Au_Device.h"

device_state_t g_device_state;

/*
  * @brief  设备状态初始化
  * @param  none
  * @return none	
  * @note   none
 */
void Init_Device_State(void)
{
    memset(&g_device_state, 0, sizeof(device_state_t));
    g_device_state.Battery = 100;
    g_device_state.Temperature = 30;
    g_device_state.Read_Ble_Finish = false;
    g_device_state.Paper_State = PAPER_STATUS_NORMAL;
    g_device_state.Printer_State = PRINTER_STATUS_INIT;
}

/*
  * @brief  获取设备状态
  * @param  none
  * @return none	
  * @note   none
 */
device_state_t * Get_Device_State(void)
{
    return &g_device_state;
}

/*
  * @brief  设置Ble接收完成标志
  * @param  Finish:true/false
  * @return none	
  * @note   none
 */
void Set_Read_Ble_Finish(bool Finish)
{
    g_device_state.Read_Ble_Finish = Finish;
}

/*
  * @brief  设置缺纸状态标志
  * @param  paper_state_t:PAPER_STATUS_NORMAL/PAPER_STATUS_LACK
  * @return none	
  * @note   none
 */
void Set_Device_Paper_Status(paper_state_t Status)
{
    g_device_state.Paper_State = Status;
}
