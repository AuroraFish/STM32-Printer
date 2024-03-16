#include "Au_Ble.h"
#include "Au_Printer.h"
#include "Au_Config.h"

bool BleConnected = true;
uint32_t PackCount = 0;

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

//AT指令
char * Ble_In_At_Mode = "+++";                      //进入AT模式
char * Ble_Set_Status = "AT+STATUS=0\r\n";          //关闭设备状态显示
char * Ble_Query_Status = "AT+STATUS?\r\n";         //查询显示设备状态功能的当前状态
char * Ble_Query_Name = "AT+NAME?\r\n";             //查询设备名称
char * Ble_Set_Name = "AT+NAME=Mini-Printer\r\n"; //设置设备名称
char * Ble_Out_At_Mode = "AT+EXIT\r\n";


//状态
typedef enum {
    BLE_INIT_START = 0,
    BLE_IN_AT_MODE,
    BLE_IN_AT_MODE_SUCCESS,
    BLE_CLOSE_STATUS,
    BLE_CLOSE_STATUS_SUCCESS,
    BLE_QUERY_STATUS,
    BLE_QUERY_STATUS_SUCCESS,
    BLE_QUERY_NAME,
    BLE_NEED_SET_NAME,
    BLE_NONEED_SET_NAME,
    BLE_SET_NAME,
    BLE_SET_NAME_SUCCESS,
    BLE_OUT_AT_MODE,
    BLE_INIT_FINISH,
    BLE_RESET,
}e_ble_init_step;

//初始状态
e_ble_init_step g_ble_init_step = BLE_INIT_START;

bool Need_Reboot_Ble = false;

/*
  * @brief  清空接受包统计
  * @param  none
  * @return none	
  * @note   none
 */
void Clean_Blepack_Count(void)
{
    PackCount = 0;
}

/*
  * @brief  获取接受包统计
  * @param  none
  * @return none	
  * @note   none
 */
uint32_t Get_Blepack_Count(void)
{
    return PackCount;
}

/*
  * @brief  获取连接状态
  * @param  none
  * @return none	
  * @note   none
 */
bool Get_Ble_Connect(void)
{
    return BleConnected;
}

/*
  * @brief  上报设备状态
  * @param  none
  * @return none	
  * @note   none
 */
void Ble_Report(void)
{
    if (Get_Ble_Connect()) {
        device_state_t * pDevice = Get_Device_State();
        uint8_t Status[4];
        Status[0] = pDevice->Battery;
        Status[1] = pDevice->Temperature;
        Status[2] = pDevice->Paper_State;
        Status[3] = pDevice->Printer_State;
        HAL_UART_Transmit(&huart2, (uint8_t*)&Status, sizeof(Status), 0xffff);
    }
}

int Cmd_Index = 0;
uint8_t Cmd_Buffer[100];
bool Need_Clean_Ble_Status = false;

/*
  * @brief  处理串口接收到的数据
  * @param  Data
  * @return none	
  * @note   none
 */
void uart_cmd_handle(uint8_t Data)
{
    Cmd_Buffer[Cmd_Index++] = Data;
    char * ptr_char = (char*)Cmd_Buffer;
    
    //解析数据
    if (g_ble_init_step == BLE_INIT_FINISH) {
        //清除如下设备状态信息
        if (strstr(ptr_char, "CONNECTED") != NULL) {
            Need_Clean_Ble_Status = true;
        }
        if (strstr(ptr_char, "DEVICE ERROR") != NULL) {
            Need_Clean_Ble_Status = true;
        }
        if (strstr(ptr_char, "DISCONNECTED") != NULL) {
            Need_Clean_Ble_Status = true;
        }
        
        //处理index为5的
        if (Cmd_Index == 5) {
            //设置打印热密度
            if (Cmd_Buffer[0] == 0xA5 && Cmd_Buffer[1] == 0xA5 && Cmd_Buffer[2] == 0xA5 && Cmd_Buffer[3] == 0xA5) {
                if (Cmd_Buffer[4] == 1) {
                    Set_Heat_Density(30);
                }
                else if (Cmd_Buffer[4] == 2) {
                    Set_Heat_Density(60);
                }
                else {
                    Set_Heat_Density(100);
                }
                Cmd_Index = 0;
                memset(Cmd_Buffer, 0, sizeof(Cmd_Buffer));
                return ; 
            }
            
            //
            if (Cmd_Buffer[0] == 0xA6 && Cmd_Buffer[1] == 0xA6 && Cmd_Buffer[2] == 0xA6 && Cmd_Buffer[3] == 0xA6) {            
                Set_Read_Ble_Finish(true);
                PRINT_INFO("ReadFinish 1 = %d", PackCount);
                Cmd_Index = 0;
                memset(Cmd_Buffer, 0, sizeof(Cmd_Buffer));
                return;
            }
        }
        
        //
        if (Cmd_Index >= 5) {
            if (Cmd_Buffer[Cmd_Index-1] == 0x01) {
                if (Cmd_Buffer[Cmd_Index-2] == 0xA6 && Cmd_Buffer[Cmd_Index-3] == 0xA6 && Cmd_Buffer[Cmd_Index-4] == 0xA6 && Cmd_Buffer[Cmd_Index-5] == 0xA6) {
                    PRINT_INFO("ReadFinish 2 = %d", PackCount);
                    Cmd_Index = 0;
                    memset(Cmd_Buffer, 0, sizeof(Cmd_Buffer));
                    Set_Read_Ble_Finish(true);
                    return ;
                }
            }
        }
        
        //将数据写入缓冲区
        if (Cmd_Index >= 48) {
            PackCount++;
            Write_To_PrintBuffer(Cmd_Buffer, Cmd_Index);
            Cmd_Index = 0;
            memset(Cmd_Buffer, 0, sizeof(Cmd_Buffer));
            
        }
    }
    // 初始化
    else {
        if (strstr(ptr_char, "OK\r\n") != NULL) {
            if (g_ble_init_step == BLE_IN_AT_MODE) {
                g_ble_init_step = BLE_IN_AT_MODE_SUCCESS;
            }
            else if (g_ble_init_step == BLE_CLOSE_STATUS) {
                g_ble_init_step = BLE_CLOSE_STATUS_SUCCESS;
            }
            else if (g_ble_init_step == BLE_QUERY_NAME) {
                if (strstr(ptr_char, "RF-CRAZY") != NULL) {
                    g_ble_init_step = BLE_NEED_SET_NAME;
                }
                else {
                    g_ble_init_step = BLE_NONEED_SET_NAME;
                }
            }
            else if (g_ble_init_step == BLE_SET_NAME) {
                g_ble_init_step = BLE_SET_NAME_SUCCESS;
            }
            else if (g_ble_init_step == BLE_OUT_AT_MODE) {
                g_ble_init_step = BLE_INIT_FINISH;
            }
            else if (g_ble_init_step == BLE_RESET) {
                g_ble_init_step = BLE_INIT_START;
            }
            else if (g_ble_init_step == BLE_QUERY_STATUS) {
                if (strstr(ptr_char, "AT+STATUS=0") != NULL) {
                    g_ble_init_step = BLE_QUERY_STATUS_SUCCESS;
                }
                else {
                    g_ble_init_step = BLE_CLOSE_STATUS;
                }
            }
            Cmd_Index = 0;
            memset(Cmd_Buffer, 0, sizeof(Cmd_Buffer));
            return ;
        }
        if (strstr(ptr_char, "ERROR\r\n") != NULL) {
            g_ble_init_step = BLE_RESET;
        }
        
        if (Cmd_Index >= sizeof(Cmd_Buffer)) {
            Cmd_Index = 0;
        }
    }
}

int Retry_Count = 0;
/*
  * @brief  蓝牙模组初始化
  * @param  none
  * @return none	
  * @note   关闭各种设备显示信息
 */
void Init_Ble(void)
{
    while (1) {
        Retry_Count++;
        vTaskDelay(50);
        
        if (g_ble_init_step == BLE_INIT_START || g_ble_init_step == BLE_IN_AT_MODE) {
            PRINT_INFO("BLE:Enter AT_MODE");
            HAL_UART_Transmit(&huart2, (uint8_t*)Ble_In_At_Mode, strlen(Ble_In_At_Mode), 0xffff);
            g_ble_init_step = BLE_IN_AT_MODE;
        }
        else if (g_ble_init_step == BLE_IN_AT_MODE_SUCCESS || g_ble_init_step == BLE_CLOSE_STATUS) {
            PRINT_INFO("BLE:Set Status(0), Close Device Status");
            HAL_UART_Transmit(&huart2, (uint8_t*)Ble_Set_Status, strlen(Ble_Set_Status), 0xffff);
            g_ble_init_step = BLE_CLOSE_STATUS;
        }
        else if (g_ble_init_step == BLE_CLOSE_STATUS_SUCCESS || g_ble_init_step == BLE_QUERY_STATUS) {
            PRINT_INFO("BLE:Query Close Device Status");
            HAL_UART_Transmit(&huart2, (uint8_t*)Ble_Query_Status, strlen(Ble_Query_Status), 0xffff);
            g_ble_init_step = BLE_QUERY_STATUS;
        }
        else if (g_ble_init_step == BLE_QUERY_STATUS_SUCCESS || g_ble_init_step == BLE_QUERY_NAME) {
            PRINT_INFO("BLE:Query Device Name");
            HAL_UART_Transmit(&huart2, (uint8_t*)Ble_Query_Name, strlen(Ble_Query_Name), 0xffff);
            g_ble_init_step = BLE_QUERY_NAME;
        }
        else if (g_ble_init_step == BLE_NEED_SET_NAME || g_ble_init_step == BLE_SET_NAME) {
            PRINT_INFO("BLE:Set Device Name");
            HAL_UART_Transmit(&huart2, (uint8_t*)Ble_Set_Name, strlen(Ble_Set_Name), 0xffff);
            g_ble_init_step = BLE_SET_NAME;
            Need_Reboot_Ble = true;
        }
        else if (g_ble_init_step == BLE_SET_NAME_SUCCESS || g_ble_init_step == BLE_NONEED_SET_NAME || g_ble_init_step == BLE_OUT_AT_MODE) {
            PRINT_INFO("BLE:Quit AT_MODE");
            HAL_UART_Transmit(&huart2, (uint8_t*)Ble_Out_At_Mode, strlen(Ble_Out_At_Mode), 0xffff);
            g_ble_init_step = BLE_OUT_AT_MODE;
        } 
        else if (g_ble_init_step == BLE_INIT_FINISH) {
            PRINT_INFO("Quit Ble Init");
            break;
        }
        else if (g_ble_init_step == BLE_RESET) {
            PRINT_INFO("BLE:BLE RESET Quit AT_MODE");
            HAL_UART_Transmit(&huart2, (uint8_t*)Ble_Out_At_Mode, strlen(Ble_Out_At_Mode), 0xffff);
            //观察一下g_ble_init_step = BLE_INIT_START;
        }
        PRINT_INFO("G_BLE_INIT_STEP = %d", g_ble_init_step);
        Run_Led(LED_BLE_INIT);
    }
    if (Need_Reboot_Ble) {
        PRINT_INFO("配置完成, 重启使用");
    }
    else {
        PRINT_INFO("配置完成, 正常使用");
    }
    
    vTaskDelay(1000);
    Cmd_Index = 0;
    memset(Cmd_Buffer, 0, sizeof(Cmd_Buffer));
}

/*
  * @brief  清理无法关闭的设备状态信息
  * @param  none
  * @return none	
  * @note   DISCONNECTED, CONNECTED, DEVICE ERROR
 */
void Ble_Status_Data_Clean(void)
{
    if (Need_Clean_Ble_Status) {
        vTaskDelay(200);
        PRINT_INFO("Clean --- %s", Cmd_Buffer);
        Cmd_Index = 0;
        memset(Cmd_Buffer, 0, sizeof(Cmd_Buffer));
        Need_Clean_Ble_Status = false;
    }
}
