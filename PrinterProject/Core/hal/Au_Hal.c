#include "Au_Hal.h"
#include "Au_Adc.h"
#include "main.h"

//近似0
#define TempZero 1e-7
//是否缺纸
bool Need_Report = false;

/*
  * @brief  将电压值转换到0~100的数值
  * @param  电压值, 转换范围
  * @return none	
  * @note   none
 */
static int map(int x, int in_min, int in_max, int out_min, int out_max) 
{     
    int Delta = x - in_min;
    int Divisor = in_max - in_min;
    int Mul = out_max - out_min;
    int result = (float)Delta/Divisor*Mul;
    
    if (result < 0) 
        result = 0;
    
    return result;
}

/*
  * @brief  读取电量值
  * @param  none
  * @return none	
  * @note   none
 */
void Read_Battery(void)
{
    Get_Device_State()->Battery = map(Get_Adc_Volts(),2050,2540,0,100);
    if (Get_Device_State()->Battery > 100)
        Get_Device_State()->Battery = 100;
    PRINT_DEBUG("Battery = %d", Get_Device_State()->Battery);
}

/*
  * @brief  读取温度值
  * @param  none
  * @return none	
  * @note   none
 */
void Read_Temperature(void)
{
    float Temperature = Get_Adc_Temperatrue();
    PRINT_DEBUG("Temperature = %f", Temperature);
    if (Temperature >= TempZero) {
        Get_Device_State()->Temperature = Temperature;
    }
    else {
        Get_Device_State()->Temperature = 0;
    }
}

/*
  * @brief  读取引脚电平
  * @param  pin
  * @return none	
  * @note   none
 */
static int Digital_Read(int pin) 
{
    if (pin == PIN_PAPER) {
        return HAL_GPIO_ReadPin(VHINT_GPIO_Port, VHINT_Pin);
    }
    return HAL_GPIO_ReadPin(VHINT_GPIO_Port, VHINT_Pin);
}

/*
  * @brief  引脚写入电平
  * @param  pin, pinState
  * @return none	
  * @note   none
 */
static void Digital_Write(int pin, int pinState)
{
    if (pin == PIN_LED) {
        HAL_GPIO_WritePin(GPIOB, LED_Pin, (GPIO_PinState)pinState);
    }
}

/*
  * @brief  蜂鸣器运行(未使用)
  * @param  beep_state_t
  * @return none	
  * @note   none
 */
void Run_Beep(beep_state_t Status)
{
    // ......
}

/*
  * @brief  LED指示灯运行
  * @param  led_state_t
  * @return none	
  * @note   警告:闪烁3次, 其他状态:闪烁1次
 */
void Run_Led(led_state_t Status)
{
    switch(Status) {
        case LED_CONNECT:
            Digital_Write(PIN_LED, 0);
            us_delay(100);
            Digital_Write(PIN_LED, 1);
            break;
        
        case LED_WARN:
            Digital_Write(PIN_LED, 0);
            us_delay(100);
            Digital_Write(PIN_LED, 1);
            us_delay(100);
            Digital_Write(PIN_LED, 0);
            us_delay(100);
            Digital_Write(PIN_LED, 1);
            us_delay(100);
            Digital_Write(PIN_LED, 0);
            us_delay(100);
            Digital_Write(PIN_LED, 1);
            break;
        
        case LED_PRINTER_START:
            Digital_Write(PIN_LED, 0);
            us_delay(100);
            Digital_Write(PIN_LED, 1);
            break;
        
        case LED_BLE_INIT:
            Digital_Write(PIN_LED, 0);
            us_delay(100);
            Digital_Write(PIN_LED, 1);
        
        default:
            break;
    }
}

/*
  * @brief  缺纸触发外部中断
  * @param  none
  * @return none	
  * @note   none
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin & GPIO_PIN_8) {
        PRINT_DEBUG("Paper lack exti!");
        Need_Report = true;
        Set_Device_Paper_Status(PAPER_STATUS_LACK);
    }
}

/*
  * @brief  读取缺纸状态
  * @param  none
  * @return none	
  * @note   none
 */
void Read_Paper_Statue(void)
{
    if (Digital_Read(PIN_PAPER) == PAPER_STATUS_NORMAL) {
        Get_Device_State()->Paper_State = PAPER_STATUS_NORMAL;
    }
    else {
        Get_Device_State()->Paper_State = PAPER_STATUS_LACK;
    }
    PRINT_DEBUG("Paper State = %d", Get_Device_State()->Paper_State);
}

/*
  * @brief  读取各部件参数
  * @param  none
  * @return none	
  * @note   电量, 温度, 缺纸状态
 */
void Read_All_Hal(void)
{
    Read_Battery();
    Read_Temperature();
    Read_Paper_Statue();
}

/*
  * @brief  初始化
  * @param  none
  * @return none	
  * @note   也没啥好初始化的
 */
void Init_Hal(void)
{
    Digital_Write(PIN_LED, 1);
}

/*
  * @brief  是否需要上报缺纸状态
  * @param  none
  * @return none	
  * @note   none
 */
bool Read_Paper_Irq_Need_Report_Status(void)
{
    if (Need_Report) {
        Need_Report = false;
        return true;
    }
    else 
        return false;
}
