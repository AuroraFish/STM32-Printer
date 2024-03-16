/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define POWER_ADC_Pin GPIO_PIN_0
#define POWER_ADC_GPIO_Port GPIOA
#define TEMPERATURE_ADC_Pin GPIO_PIN_1
#define TEMPERATURE_ADC_GPIO_Port GPIOA
#define USART2_BLE_RX_Pin GPIO_PIN_2
#define USART2_BLE_RX_GPIO_Port GPIOA
#define USART2_BLE_TX_Pin GPIO_PIN_3
#define USART2_BLE_TX_GPIO_Port GPIOA
#define BTN_Pin GPIO_PIN_4
#define BTN_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOB
#define LAT_Pin GPIO_PIN_1
#define LAT_GPIO_Port GPIOB
#define STB1_Pin GPIO_PIN_10
#define STB1_GPIO_Port GPIOB
#define STB2_Pin GPIO_PIN_11
#define STB2_GPIO_Port GPIOB
#define STB3_Pin GPIO_PIN_12
#define STB3_GPIO_Port GPIOB
#define STB4_Pin GPIO_PIN_13
#define STB4_GPIO_Port GPIOB
#define STB5_Pin GPIO_PIN_14
#define STB5_GPIO_Port GPIOB
#define STB6_Pin GPIO_PIN_15
#define STB6_GPIO_Port GPIOB
#define VHINT_Pin GPIO_PIN_8
#define VHINT_GPIO_Port GPIOA
#define VHINT_EXTI_IRQn EXTI9_5_IRQn
#define USART1_LOG_TX_Pin GPIO_PIN_9
#define USART1_LOG_TX_GPIO_Port GPIOA
#define USART1_LOG_RX_Pin GPIO_PIN_10
#define USART1_LOG_RX_GPIO_Port GPIOA
#define VH_EN_Pin GPIO_PIN_11
#define VH_EN_GPIO_Port GPIOA
#define MOTOR_IN4_Pin GPIO_PIN_6
#define MOTOR_IN4_GPIO_Port GPIOB
#define MOTOR_IN3_Pin GPIO_PIN_7
#define MOTOR_IN3_GPIO_Port GPIOB
#define MOTOR_IN2_Pin GPIO_PIN_8
#define MOTOR_IN2_GPIO_Port GPIOB
#define MOTOR_IN1_Pin GPIO_PIN_9
#define MOTOR_IN1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
