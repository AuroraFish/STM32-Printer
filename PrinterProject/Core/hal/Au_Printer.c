#include "Au_Printer.h"
#include "Au_Timer.h"
#include "Debug.h"

float addTime[6] = {0};
//点数-增加时间系数
#define kAddTime 0.001

//根据打印头实际打印效果修改打印时间偏移值
#define STB1_ADDTIME 0
#define STB2_ADDTIME 0
#define STB3_ADDTIME 0
#define STB4_ADDTIME 0
#define STB5_ADDTIME 0
#define STB6_ADDTIME 0

//热密度
uint8_t Heat_Density = 64;

/*
  * @brief  引脚写入电平
  * @param  pin:引脚, pinState:引脚状态
  * @return none	
  * @note   none
 */
static void Digital_Write(int pin, int pinState)
{
    if (pin == PIN_STB1) {
        HAL_GPIO_WritePin(GPIOB, STB1_Pin, (GPIO_PinState)pinState);
    }
    else if (pin == PIN_STB2) {
        HAL_GPIO_WritePin(GPIOB, STB2_Pin, (GPIO_PinState)pinState);
    }
    else if (pin == PIN_STB3) {
        HAL_GPIO_WritePin(GPIOB, STB3_Pin, (GPIO_PinState)pinState);
    }
    else if (pin == PIN_STB4) {
        HAL_GPIO_WritePin(GPIOB, STB4_Pin, (GPIO_PinState)pinState);
    }
    else if (pin == PIN_STB5) {
        HAL_GPIO_WritePin(GPIOB, STB5_Pin, (GPIO_PinState)pinState);
    }
    else if (pin == PIN_STB6) {
        HAL_GPIO_WritePin(GPIOB, STB6_Pin, (GPIO_PinState)pinState);
    }
    else if (pin == PIN_LAT) {
        HAL_GPIO_WritePin(GPIOB, LAT_Pin, (GPIO_PinState)pinState);
    }
}

/*
  * @brief  VHEN引脚写入电平
  * @param  pin, pinState
  * @return none	
  * @note   none
 */
static void Digital_Write_Vhen(int pin,int PinState){
    PRINT_DEBUG("START VHEN");
	HAL_GPIO_WritePin(GPIOA,VH_EN_Pin,(GPIO_PinState)PinState);
}

/*
  * @brief  失能所有通道
  * @param  none
  * @return none	
  * @note   none
 */
static void Set_Stb_Unable(void)
{
    Digital_Write(PIN_STB1, LOW);
    Digital_Write(PIN_STB2, LOW);
    Digital_Write(PIN_STB3, LOW);
    Digital_Write(PIN_STB4, LOW);
    Digital_Write(PIN_STB5, LOW);
    Digital_Write(PIN_STB6, LOW);
}

/*
  * @brief  打印前初始化
  * @param  none
  * @return none	
  * @note   none
 */
static void Init_Printing(void)
{
    PRINT_DEBUG("Init Printing!");
    //开启打印超时监听
    Open_Printer_Timeout_Timer();
    //STB, LAT, VHEN ON
    Set_Stb_Unable();
    Digital_Write(PIN_LAT, HIGH);
    Digital_Write_Vhen(PIN_VHEN, HIGH);
}

/*
  * @brief  打印后停止
  * @param  none
  * @return none	
  * @note   none
 */
static void Stop_Printing(void)
{
    PRINT_DEBUG("Stop Printing!");
    //关闭打印超时监听
    Close_Printer_Timeout_Timer();
    //STB, LAT, VHEN OFF
    Digital_Write_Vhen(PIN_VHEN, LOW);
    Set_Stb_Unable();
    Digital_Write(PIN_LAT, HIGH);
}

/*
  * @brief  设置打印热密度
  * @param  Density:热密度
  * @return none	
  * @note   none
 */
void Set_Heat_Density(uint8_t Density)
{
    PRINT_DEBUG("打印密度设置%d\n", Density);
    Heat_Density = Density;
}

void Clear_AddTime(void)
{
    addTime[0] = addTime[1] = addTime[2] = addTime[3] = addTime[4] = addTime[5] = 0;
}

/*
  * @brief  发送一行数据
  * @param  data:数据
  * @return none	
  * @note   根据数据动态调整打印时间
 */
static void Send_ALine_Data(uint8_t* data)
{
    PRINT_DEBUG("Start send a line Data!"); 
    float tmpAddTime = 0;
    Clear_AddTime();
    
    for (uint8_t i = 0; i < 6; ++i) {
        for (uint8_t j = 0; j<8; ++j) {
            addTime[i] += data[i*8+j];
        }
        tmpAddTime = addTime[i] * addTime[i];
        addTime[i] = kAddTime * tmpAddTime;
    }
    
    Spi_Command(data, TPH_DI_LEN);
    Digital_Write(PIN_LAT, LOW);
    us_delay(LAT_TIME);
    Digital_Write(PIN_LAT, HIGH);
}

/*
  * @brief  通道打印运行
  * @param  当前通道号
  * @return none	
  * @note   none
 */
static void Run_STB(uint8_t Now_STB_num)
{
    PRINT_DEBUG("Start Run STB!");
    switch (Now_STB_num) {
        case 0:
            Digital_Write(PIN_STB1, 1);
            us_delay((PRINT_TIME + addTime[0] + STB1_ADDTIME) * ((double)Heat_Density / 100));
            Digital_Write(PIN_STB1, 0);
            us_delay(PRINT_END_TIME);
            break;
        case 1:
            Digital_Write(PIN_STB2, 1);
            us_delay((PRINT_TIME + addTime[1] + STB2_ADDTIME) * ((double)Heat_Density / 100));
            Digital_Write(PIN_STB2, 0);
            us_delay(PRINT_END_TIME);
            break;
        case 2:
            Digital_Write(PIN_STB3, 1);
            us_delay((PRINT_TIME + addTime[2] + STB3_ADDTIME) * ((double)Heat_Density / 100));
            Digital_Write(PIN_STB3, 0);
            us_delay(PRINT_END_TIME);
            break;
        case 3:
            Digital_Write(PIN_STB4, 1);
            us_delay((PRINT_TIME + addTime[3] + STB4_ADDTIME) * ((double)Heat_Density / 100));
            Digital_Write(PIN_STB4, 0);
            us_delay(PRINT_END_TIME);
            break;
        case 4:
            Digital_Write(PIN_STB5, 1);
            us_delay((PRINT_TIME + addTime[4] + STB5_ADDTIME) * ((double)Heat_Density / 100));
            Digital_Write(PIN_STB5, 0);
            us_delay(PRINT_END_TIME);
            break;
        case 5:
            Digital_Write(PIN_STB6, 1);
            us_delay((PRINT_TIME + addTime[5] + STB6_ADDTIME) * ((double)Heat_Density / 100));
            Digital_Write(PIN_STB6, 0);
            us_delay(PRINT_END_TIME);
            break;
        default:
            break;
    }
}

/*
  * @brief  移动电机&开始打印
  * @param  needstop, STBnum
  * @return none	
  * @note   none
 */
bool Move_And_Start_STB(bool need_stop, uint8_t STBnum)
{
    if (need_stop == true) {
        PRINT_DEBUG("stop printing!");
        Motor_Stop();
        Stop_Printing();
        return true;
    }
    
    //4Steps a line
    Motor_Run();
    if (STBnum == ALL_STB_NUM) {
        //所有通道打印
        for (uint8_t index = 0; index<6; index++) {
            Run_STB(index);
            
            //在通道加热的同时将电机运行信号插入
            if (index == 1 || index == 3 || index == 5) {
                Motor_Run();
            }
        }
        
    }
    else {
        //单通道打印
        Run_STB(STBnum);
        Motor_Run_Steps(3);
    }
    
    return false;
}

/*
  * @brief  打印错误检查
  * @param  Need_Report:是否需要BLE上报
  * @return none	
  * @note   none
 */
bool Printing_Error_Check(bool Need_Report)
{
    if (Get_Printer_Timeout_Status()) {
        PRINT_DEBUG("Printing_Error_Check: TimerOut!");
        return true;
    }
    
    if (Get_Device_State()->Paper_State == PAPER_STATUS_LACK) {
        if (Need_Report) {
            Clean_PrintBuffer();
            Ble_Report();
        }
        
        PRINT_DEBUG("Printing_Error_Check: PaperLack!");
        //Run_Beep(BEEP_WARN); 未使用
        Run_Led(LED_WARN);
        return true;
    }
    
    if (Get_Device_State()->Temperature > 65) {
        if (Need_Report) {
            Clean_PrintBuffer();
            Ble_Report();
        }
        
        PRINT_DEBUG("Printing_Error_Check: TemperError!");
        Run_Led(LED_WARN);
        
        return true;
    }
    
    return false;
}

/*
  * @brief  队列缓冲区打印
  * @param  none 
  * @return none	
  * @note   none
 */
void Start_Printing_By_QueueBuffer(void)
{
    uint8_t * pData = NULL;
    uint32_t PrinterCount = 0;
    Init_Printing();
    
    while (1) {
        if (Get_ble_rx_leftline() > 0) {
            //从缓冲区读取一行数据
            pData = Read_To_PrintBuffer();
            if (pData != NULL) {
                PrinterCount++;
                Send_ALine_Data(pData);
                if (Move_And_Start_STB(false, ALL_STB_NUM)) {
                    break;
                }
            }
        }
        else {
            //停止打印
            if (Move_And_Start_STB(true, ALL_STB_NUM)) {
                break;
            }
        }
        
        if (Get_Printer_Timeout_Status()) {
            break;
        }
        if (Printing_Error_Check(true)) {
            break;
        }
    }
    
    Motor_Run_Steps(100);
    Motor_Stop();
    Clean_Blepack_Count();
    PRINT_DEBUG("Start_Printing_By_QueueBuffer: PrintingFinish, Recv: %d", PrinterCount);
}
  
/*
  * @brief  单通道打印
  * @param  STBnum:通道号, Data:数据
  * @return none	
  * @note   none
 */
void Start_Printing_By_OneSTB(uint8_t STBnum, uint8_t* Data, uint32_t Len)
{
    PRINT_DEBUG("Start Printing By One STB!");
    uint32_t offset = 0;
    uint8_t* ptr = Data;
    bool need_stop = false;
    Init_Printing();
    
    while (1) {
        PRINT_DEBUG("printer %d", offset);
        if (Len > offset) {
            //发送一行数据 48Byte
            Send_ALine_Data(ptr);
            offset += TPH_DI_LEN;
            ptr += TPH_DI_LEN;
        }
        else {
            need_stop = true;
        }
        
        if (Move_And_Start_STB(need_stop, STBnum)) {
            break;
        }
        if (Get_Printer_Timeout_Status()) {
            break;
        }
        if (Printing_Error_Check(false)) {
            break;
        }
    }
    Motor_Run_Steps(40);
    Motor_Stop();
}

/*
  * @brief  设置TESTSTB数据
  * @param  Print_Data:传入数组
  * @return none	
  * @note   none
 */
static void Set_Debug_Data(uint8_t* Print_Data)
{
    for (uint32_t len = 0; len < 48*5; len++){
        Print_Data[len] = 0x55;
    }
    PRINT_DEBUG("Finish Set Debug Data");
}

/*
  * @brief  测试打印模块
  * @param  none
  * @return none	
  * @note   none
 */
void TestSTB(void)
{
    uint8_t Print_Data[48*6];
    //每行48个字节，48*8=384个像素点，打印5行
    uint32_t Print_Len;
    Print_Len = 48 * 5;
    PRINT_DEBUG("Start TestSTB, Sequence:123456!");
    
    Set_Debug_Data(Print_Data);
    Start_Printing_By_OneSTB(0, Print_Data, Print_Len);
    Set_Debug_Data(Print_Data);
    Start_Printing_By_OneSTB(1, Print_Data, Print_Len);
    Set_Debug_Data(Print_Data);
    Start_Printing_By_OneSTB(2, Print_Data, Print_Len);
    Set_Debug_Data(Print_Data);
    Start_Printing_By_OneSTB(3, Print_Data, Print_Len);
    Set_Debug_Data(Print_Data);
    Start_Printing_By_OneSTB(4, Print_Data, Print_Len);
    Set_Debug_Data(Print_Data);
    Start_Printing_By_OneSTB(5, Print_Data, Print_Len);
    
    PRINT_DEBUG("Finish TestSTB~");
}

/*
  * @brief  打印模块初始化
  * @param  none
  * @return none	
  * @note   none
 */
void Init_Printer(void)
{
    Init_Motor();
    Set_Stb_Unable();
    Digital_Write_Vhen(PIN_VHEN, RESET);
    Init_Spi();
}
