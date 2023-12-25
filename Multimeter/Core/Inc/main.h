/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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
#define AC_VOLTAGE_MODE_Pin GPIO_PIN_0
#define AC_VOLTAGE_MODE_GPIO_Port GPIOC
#define AC_VOLTAGE_MODE_EXTI_IRQn EXTI0_IRQn
#define DC_VOLTAGE_MODE_RANGE_1_Pin GPIO_PIN_1
#define DC_VOLTAGE_MODE_RANGE_1_GPIO_Port GPIOC
#define DC_VOLTAGE_MODE_RANGE_1_EXTI_IRQn EXTI1_IRQn
#define DC_VOLTAGE_MODE_RANGE_2_Pin GPIO_PIN_2
#define DC_VOLTAGE_MODE_RANGE_2_GPIO_Port GPIOC
#define DC_VOLTAGE_MODE_RANGE_2_EXTI_IRQn EXTI2_IRQn
#define DC_VOLTAGE_MODE_RANGE_3_Pin GPIO_PIN_3
#define DC_VOLTAGE_MODE_RANGE_3_GPIO_Port GPIOC
#define DC_VOLTAGE_MODE_RANGE_3_EXTI_IRQn EXTI3_IRQn
#define FREQUENCY_INPUT_Pin GPIO_PIN_0
#define FREQUENCY_INPUT_GPIO_Port GPIOA
#define VOLTAGE_INPUT_Pin GPIO_PIN_1
#define VOLTAGE_INPUT_GPIO_Port GPIOA
#define RESISTANCE_INPUT_Pin GPIO_PIN_2
#define RESISTANCE_INPUT_GPIO_Port GPIOA
#define RESISTANCE_MODE_RANGE_1_Pin GPIO_PIN_4
#define RESISTANCE_MODE_RANGE_1_GPIO_Port GPIOC
#define RESISTANCE_MODE_RANGE_1_EXTI_IRQn EXTI4_IRQn
#define RESISTANCE_MODE_RANGE_2_Pin GPIO_PIN_5
#define RESISTANCE_MODE_RANGE_2_GPIO_Port GPIOC
#define RESISTANCE_MODE_RANGE_2_EXTI_IRQn EXTI9_5_IRQn
#define RESISTANCE_MODE_RANGE_3_Pin GPIO_PIN_6
#define RESISTANCE_MODE_RANGE_3_GPIO_Port GPIOC
#define RESISTANCE_MODE_RANGE_3_EXTI_IRQn EXTI9_5_IRQn
#define CAPACITY_MODE_RANGE_1_Pin GPIO_PIN_7
#define CAPACITY_MODE_RANGE_1_GPIO_Port GPIOC
#define CAPACITY_MODE_RANGE_1_EXTI_IRQn EXTI9_5_IRQn
#define CAPACITY_MODE_RANGE_2_Pin GPIO_PIN_8
#define CAPACITY_MODE_RANGE_2_GPIO_Port GPIOC
#define CAPACITY_MODE_RANGE_2_EXTI_IRQn EXTI9_5_IRQn
#define CAPACITY_MODE_RANGE_3_Pin GPIO_PIN_9
#define CAPACITY_MODE_RANGE_3_GPIO_Port GPIOC
#define CAPACITY_MODE_RANGE_3_EXTI_IRQn EXTI9_5_IRQn
#define DISPLAY_I2C_SCL_Pin GPIO_PIN_6
#define DISPLAY_I2C_SCL_GPIO_Port GPIOB
#define DISPLAY_I2C_SDA_Pin GPIO_PIN_7
#define DISPLAY_I2C_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
