#include "Au_queue.h"
#include "semphr.h"

ble_rx_t g_ble_rx;

//互斥访问缓冲区
SemaphoreHandle_t xHandler = NULL;

/*
  * @brief  初始化
  * @param  none
  * @return none	
  * @note   none
 */
void Init_Queue(void)
{
    Clean_PrintBuffer();
    xHandler = xSemaphoreCreateMutex();
}

/*
  * @brief  清空接收缓存
  * @param  none
  * @return none	
  * @note   none
 */
void Clean_PrintBuffer(void)
{
    g_ble_rx.w_index = 0;
    g_ble_rx.r_index = 0;
    g_ble_rx.left_line = 0;
}

/*
  * @brief  写入一行数据
  * @param  pData, Length
  * @return none	
  * @note   none
 */
void Write_To_PrintBuffer(uint8_t * pData, size_t Length)
{
    //通知调度器是否需要立即切换到更高优先级的任务
    static BaseType_t xHigherPriorityTaskWoken = false;
    
    if (Length == 0) {
        return ;
    }
    if (g_ble_rx.left_line >= MAX_LINE) {
        return ;
    }
    if (Length > MAX_ONELINE_BYTE) {
        Length = MAX_ONELINE_BYTE;
    }
    
    //获取信号量
    if (xSemaphoreTakeFromISR(xHandler, &xHigherPriorityTaskWoken) == pdPASS) {
        memcpy(&g_ble_rx.Printer_Buffer[g_ble_rx.w_index], pData, Length);
        g_ble_rx.w_index++;
        g_ble_rx.left_line++;
        
        if (g_ble_rx.w_index >= MAX_LINE) {
            g_ble_rx.w_index = 0;
        }    
        if (g_ble_rx.left_line >= MAX_LINE) {
            g_ble_rx.left_line = MAX_LINE;
        }
        
        //释放信号量
        xSemaphoreGiveFromISR(xHandler, &xHigherPriorityTaskWoken);
    }
    
    //如果有更高优先级的任务需要唤醒, 切换任务
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*
  * @brief  读取一行数据
  * @param  none
  * @return none	
  * @note   none
 */
uint8_t * Read_To_PrintBuffer(void)
{
    uint32_t index = 0;
    
    //查看是否可以获取信号量, 等待10个Tick
    if (xSemaphoreTake(xHandler, (portTickType)10) == pdPASS) {
        if (g_ble_rx.left_line > 0) {
            g_ble_rx.left_line--;
            index = g_ble_rx.r_index;
            g_ble_rx.r_index++;
            
            if (g_ble_rx.r_index >= MAX_LINE) {
                g_ble_rx.r_index = 0;
            }
            
            //释放信号量
            xSemaphoreGive(xHandler);
            return g_ble_rx.Printer_Buffer[index].buffer;
        }
        
        xSemaphoreGive(xHandler);
        return NULL;
    }
    else {
        return NULL;
    }
}

/*
  * @brief  获取Ble数据包剩余行数
  * @param  none
  * @return none	
  * @note   none
 */
uint32_t Get_ble_rx_leftline(void)
{
    return g_ble_rx.left_line;
}
