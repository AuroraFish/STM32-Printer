#include "Au_Spi.h"

/*
  * @brief  对HAL库SPI的一层封装
  * @param  data_buffer:打印数据, data_len:数据长度
  * @return none	
  * @note   none
 */
void Spi_Command(uint8_t* data_buffer, uint8_t data_len)
{
    HAL_SPI_Transmit(&hspi1, data_buffer, data_len, 0xFF);
}

/*
  * @brief  Spi初始化
  * @param  none
  * @return none	
  * @note   none
 */
void Init_Spi(void)
{
    //可以对默认SPI的设置进行修改
}
