#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>

/*
  * @brief  输出log信息, INFO, DEBUF, ERR 三个级别
  * @param  none
  * @return none	
  * @note   none
 */
#define PRINT_DEBUG_ENABLE  1
#define PRINT_ERR_ENABLE    0
#define PRINT_INFO_ENABLE   1

#if PRINT_DEBUG_ENABLE 
#define PRINT_DEBUG(fmt, args...) do{(printf("[DEBUG] >> "), printf(fmt, ##args), printf("\r\n"));}while(0)
#else
#define PRINT_DEBUG(fmt, args...)
#endif

#if PRINT_ERR_ENABLE
#define PRINT_ERR(fmt, args...) do{(printf("[ERR] >> "), printf(fmt, ##args), printf("\r\n"));}while(0)
#else
#define PRINT_ERR(fmt, args...)
#endif

#if PRINT_INFO_ENABLE
#define PRINT_INFO(fmt, args...) do{(printf("[INFO] >> "), printf(fmt, ##args), printf("\r\n"));}while(0)
#else 
#define PRINT_INFO(fmt, args...)
#endif

/*
  * @brief  断言 ASSERT
  * @param  none
  * @return none	
  * @note   none
 */
#define Assert_Called(char,int) printf("\nError:%s,%d\r\n",char,int)
#define ASSERT(X)   if((X)==0)  Assert_Called(__FILE__,__LINE__)
    
typedef enum {
    ASSERT_ERR = 0,
    ASSERT_SUCCESS = !ASSERT_ERR
} ASSERT_ERRORSTATUS;

typedef enum {
    FALSE = 0,
    TRUE = !FALSE
}RESULTSTATUS;

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

#endif
