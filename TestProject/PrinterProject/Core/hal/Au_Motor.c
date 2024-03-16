#include "Au_Motor.h"
#include "main.h"

/*
步进电机运动原理图:
       (1)              (2)              (3)              (4)
        A                A                A                A       
     \                                                       
      \
  B-   \.      B   B- ---.      B   B-    .      B   B-    .      B
                                         /                 |
                                        /                  |  
        A-               A-            /   A-              A-      
        
       (8)              (7)              (6)              (5)
        A                A  /             A                A       
        |                  /                                  
        |                 /
  B-    .      B   B-    .      B   B-    .----  B   B-    .      B
                                                             \
                                                              \ 
        A-               A-                A-              A-  \    

        IN2 IN4 IN1 IN3
step    A   B   A-  B-
        AM  BM  AP  BP
1       1   0   0   1
2       0   0   0   1
3       0   0   1   1
4       0   0   1   0
5       0   1   1   0
6       0   1   0   0
7       1   1   0   0   
8       1   0   0   0
*/

/*
char PosMap[8][78] = {
    "\r\n        A\r\n     \\\r\n      \\\r\n  B-   \\.      B\r\n\r\n\r\n        A-\r\n",
    "\r\n        A\r\n\r\n\r\n  B- ---.      B\r\n\r\n\r\n        A-\r\n",
    "\r\n        A\r\n\r\n\r\n  B-    .      B\r\n       /\r\n      /\r\n     /  A-\r\n",
    "\r\n        A\r\n\r\n\r\n  B-    .      B\r\n        |\r\n        |\r\n        A-\r\n",
    "\r\n        A\r\n\r\n\r\n  B-    .      B\r\n          \\\r\n           \\\r\n        A-  \\\r\n",
    "\r\n        A\r\n\r\n\r\n  B-    .----  B\r\n\r\n\r\n        A-\r\n",
    "\r\n        A  /\r\n          /\r\n         /\r\n  B-    .      B\r\n\r\n\r\n        A-\r\n",
    "\r\n        A\r\n        |\r\n        |\r\n  B-    .      B\r\n\r\n\r\n        A-\r\n"};
*/

uint8_t Motor_Pos = 0;

uint8_t Motor_Table[8][4] = {
    {1, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {1, 0, 0, 0}
};

osTimerId Motor_Timer;

/*
  * @brief  向电机引脚写数据, 驱动电机转动
  * @param  pin:步进电机的四相(AM, BM, AP, BP)
  * @param  PinState:0 or 1
  * @return none	
  * @note   none
 */
static void Motor_Write(int pin, int PinState)
{
    if (pin == PIN_MOTOR_AM) {
        HAL_GPIO_WritePin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin, (GPIO_PinState)PinState);
    }
    else if (pin == PIN_MOTOR_BM) {
        HAL_GPIO_WritePin(MOTOR_IN4_GPIO_Port, MOTOR_IN4_Pin, (GPIO_PinState)PinState);
    } 
   else if (pin == PIN_MOTOR_AP) {
        HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin, (GPIO_PinState)PinState);
   } 
   else if (pin == PIN_MOTOR_BP) {
        HAL_GPIO_WritePin(MOTOR_IN3_GPIO_Port, MOTOR_IN3_Pin, (GPIO_PinState)PinState);
   }
}

static void Debug_Print(void)
{
    PRINT_DEBUG("%d %d %d %d ", 
        HAL_GPIO_ReadPin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin),
        HAL_GPIO_ReadPin(MOTOR_IN4_GPIO_Port, MOTOR_IN4_Pin),
        HAL_GPIO_ReadPin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin),
        HAL_GPIO_ReadPin(MOTOR_IN3_GPIO_Port, MOTOR_IN3_Pin));
}

/*
  * @brief  获取相位
  * @param  none
  * @return 相位	
  * @note   none
 */
uint8_t Get_Phase(void)
{
    uint8_t i = 0;
    uint8_t temp[4] = {0, 0, 0, 0};
    temp[0] = HAL_GPIO_ReadPin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin);
    temp[1] = HAL_GPIO_ReadPin(MOTOR_IN4_GPIO_Port, MOTOR_IN4_Pin);
    temp[2] = HAL_GPIO_ReadPin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin);
    temp[3] = HAL_GPIO_ReadPin(MOTOR_IN3_GPIO_Port, MOTOR_IN3_Pin);
    
    for (i=0; i<8; i++) {
        if (Motor_Table[i][0] == temp[0] &&
            Motor_Table[i][1] == temp[1] &&
            Motor_Table[i][2] == temp[2] &&
            Motor_Table[i][3] == temp[3])
           return i;
    }
    
    return 8;
}

/*
  * @brief  定时器回调函数
  * @param  arg:none
  * @return none	
  * @note   none
 */
void Read_Motor_Timer_Callback(void const* arg)
{
    //PRINT_DEBUG("Motor's timerHandler is running!");
    Motor_Write(PIN_MOTOR_AM, Motor_Table[Motor_Pos][0]);
    Motor_Write(PIN_MOTOR_BM, Motor_Table[Motor_Pos][1]);
    Motor_Write(PIN_MOTOR_AP, Motor_Table[Motor_Pos][2]);
    Motor_Write(PIN_MOTOR_BP, Motor_Table[Motor_Pos][3]);
    
    //Debug_Print();
    
    Motor_Pos++;
    if (Motor_Pos >= 8) {
        Motor_Pos = 0;
        Debug_Print();
    }
}

/*
  * @brief  电机开始工作
  * @param  none
  * @return none	
  * @note   none
 */
void Motor_Start(void)
{
    PRINT_DEBUG("Motor start");
    
    if (Motor_Timer == NULL) {
        osTimerDef(Motor_timer_handler, Read_Motor_Timer_Callback);
        Motor_Timer = osTimerCreate(osTimer(Motor_timer_handler), osTimerPeriodic, NULL);
    }
    /*建议时间:2*/
    osTimerStart(Motor_Timer, 2);
}

/*
  * @brief  电机停止工作
  * @param  none
  * @return none	
  * @note   none
 */
void Motor_Stop(void)
{
    PRINT_DEBUG("Motor stop!");
    
    Motor_Write(PIN_MOTOR_AM, RESET);
    Motor_Write(PIN_MOTOR_BM, RESET);
    Motor_Write(PIN_MOTOR_AP, RESET);
    Motor_Write(PIN_MOTOR_BP, RESET);
    
    if (Motor_Timer != NULL) {
        osTimerStop(Motor_Timer);
    }
}

/*
  * @brief  手动单步运行
  * @param  none
  * @return none	
  * @note   none
 */
void Motor_Run() 
{
    PRINT_DEBUG("Motor run!");
    
    Motor_Write(PIN_MOTOR_AM, Motor_Table[Motor_Pos][0]);
    Motor_Write(PIN_MOTOR_BM, Motor_Table[Motor_Pos][1]);
    Motor_Write(PIN_MOTOR_AP, Motor_Table[Motor_Pos][2]);
    Motor_Write(PIN_MOTOR_BP, Motor_Table[Motor_Pos][3]);
    
    Debug_Print();
    
    Motor_Pos++;
    if (Motor_Pos >= 8) {
        Motor_Pos = 0;
    }
}

/*
  * @brief  手动多步运行
  * @param  steps:打印步数
  * @return none	
  * @note   none
 */
void Motor_Run_Steps(uint32_t steps)
{
    PRINT_DEBUG("Motor run steps!");
    while (steps--) {
        Motor_Write(PIN_MOTOR_AM, Motor_Table[Motor_Pos][0]);
        Motor_Write(PIN_MOTOR_BM, Motor_Table[Motor_Pos][1]);
        Motor_Write(PIN_MOTOR_AP, Motor_Table[Motor_Pos][2]);
        Motor_Write(PIN_MOTOR_BP, Motor_Table[Motor_Pos][3]); 
        
        Debug_Print();
        
        Motor_Pos++;
        if (Motor_Pos >= 8) {
            Motor_Pos = 0;
        }
        
        /* 等待一步 */
        us_delay(MOTOR_WAIT_TIME);
    }
}

/*
  * @brief  电机初始化
  * @param  none
  * @return none	
  * @note   none
 */
void Init_Motor(void)
{
    PRINT_DEBUG("Init_Motor!");
    
    Motor_Write(PIN_MOTOR_AM, RESET);
    Motor_Write(PIN_MOTOR_BM, RESET);
    Motor_Write(PIN_MOTOR_AP, RESET);
    Motor_Write(PIN_MOTOR_BP, RESET); 
    
    Debug_Print();
    PRINT_DEBUG("Init_Motor finish!");
}
