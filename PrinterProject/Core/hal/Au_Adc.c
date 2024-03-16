#include "Au_Adc.h"
#include "Au_Config.h"
#include "main.h"
#include "math.h"

#define ADC_READ_TIME 10

extern ADC_HandleTypeDef hadc1;

uint32_t Adc_Value[2];

/*
  * @brief  none
  * @param  none
  * @return none	
  * @note   none
 */
void Init_Adc(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1000);
}

/*
  * @brief  滤波算法
  * @param  adc:转换10次Adc得到的数据数组
  * @param  size:#define ADC_READ_TIME 10
  * @return 处理后的Adc数据	
  * @note   取10次Adc数据, 去掉其中的最大最小值, 然后以平均值作为此次读取到的实际数据
 */
uint32_t Adc_Alg_Handle(uint32_t* adc, int size) 
{
    int i;
    uint32_t sum_value = 0;
    uint32_t min_value = adc[0];
    uint32_t max_value = adc[0];
    
    for (i = 0; i < size; i++) {
        if (adc[i] < min_value)
            min_value = adc[i];
        else if (adc[i] > max_value)
            max_value = adc[i];
        sum_value += adc[i];
    }
    
    sum_value = sum_value - (min_value + max_value);
    uint32_t rev_value = sum_value / (size - 2);
    return rev_value;
}

/*
  * @brief  获取引脚电压值, 并滤波处理所得数据
  * @param  none
  * @return 电压	
  * @note   none
 */
int Get_Adc_Volts(void)
{
    int count;
    
    //uint32_t data = 0;
    uint32_t Adc_Ch1[ADC_READ_TIME];
    uint32_t Adc_Ch2[ADC_READ_TIME];
    
    for (count = 0; count < ADC_READ_TIME; count++) {
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
            Adc_Ch1[count] = HAL_ADC_GetValue(&hadc1);
        }
        
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
            Adc_Ch2[count] = HAL_ADC_GetValue(&hadc1);
        }
        
        HAL_ADC_Stop(&hadc1);
    }
    
    Adc_Value[0] = Adc_Alg_Handle(Adc_Ch1, ADC_READ_TIME);
    Adc_Value[1] = Adc_Alg_Handle(Adc_Ch2, ADC_READ_TIME);
    
    PRINT_DEBUG("ADC channel_0 value = %d", Adc_Value[0]);
    PRINT_DEBUG("ADC channel_1 value = %d", Adc_Value[1]);
    //data = Adc_Value[0] * 3.3f / 4096;
    
    HAL_ADC_Stop(&hadc1);
    return Adc_Value[0];
}

/*
  * @brief  根据公式, 以热敏阻值计算出实际温度
  * @param  Rt:Adc所得阻值
  * @return 温度	
  * @note   热敏电阻公式: Rt = Rp*Exp(B*(1/T1-1/T2)) -> T1 = 1/(log(Rt/Rp)/B+1/T2)
  * @note   T1和T2指的是卡尔文温度, K = 273.15 + 摄氏度
  * @note   Rt为热敏电阻在T1温度下的阻值, 其它参数见资料手册
  * @noete  最终对应的摄氏度T = T1 - 273.15, 同时+0.5的误差校正
 */
float Au_Temp_Cal(float Rt)
{
    float Rp = 30000;
    float T2 = 273.15 + 25;
    float Bx = 3950;
    float Ka = 273.15;
    float Temp = 0.0f;
    
    Temp = 1 / (log(Rt/Rp) / Bx + 1/T2 ) - Ka + 0.5;
    return Temp;
}

/*
  * @brief  获取温度
  * @param  none
  * @return 温度	
  * @note   none
 */
float Get_Adc_Temperatrue(void)
{
    float Temp = 0.0f;
    float Rt = 0;
    float Vol = 0;
    
    Vol = (float)Adc_Value[1] * 3.3f / 4096;    // Adc转换为电压值
    Rt = (Vol * 10000) / (3.3-Vol);             //电压转换为阻值, 串联分压
    
    Temp = Au_Temp_Cal(Rt);
    return Temp;
}

/*
  * @brief  测试Adc
  * @param  none
  * @return none	
  * @note   none
 */
void Au_Adc_Test(void)
{
    static uint8_t flag = 0;
    if (!flag) {
        PRINT_DEBUG("Au_Adc_Test Start!");
        flag = 1;
    }

    PRINT_DEBUG("Temper value : %d", Adc_Value[1]);
    PRINT_DEBUG("Lightt value : %d", Adc_Value[0]);
}
