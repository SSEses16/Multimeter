/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ssd1306.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*Общий поправочный коэффициент (например, если нужно пересчитать 3,3 в 2 и т.п.)
 * если не нужен, то пусть останется 1
 * */
#define K		1

#define Kc		1

#define Kr1		1
#define Kr2		1
#define Kr3		1

#define Ku1		1
#define Ku2		1
#define Ku3		1

#define Ku_a	1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

enum modes {
	AC_VOLTAGE_MODE = 0,
	DC_VOLTAGE_MODE_RANGE_1,
	DC_VOLTAGE_MODE_RANGE_2,
	DC_VOLTAGE_MODE_RANGE_3,
	RESISTANCE_MODE_RANGE_1,
	RESISTANCE_MODE_RANGE_2,
	RESISTANCE_MODE_RANGE_3,
	CAPACITY_MODE_RANGE_1,
	CAPACITY_MODE_RANGE_2,
	CAPACITY_MODE_RANGE_3,
};

enum adc_channels {
	VOLTAGE_CHANNEL = 1,
	RESISTANCE_CHANNEL
};

static char str[9] 		       = {0};

static uint32_t adc_V_buf[100] = {0};
static uint32_t adc_R_buf[100] = {0};
static uint32_t freq 		   = 0;
static uint8_t  i 			   = 0;
static uint8_t  q 			   = 0;
static uint8_t  mode 		   = 0;
static uint8_t 	channel		   = VOLTAGE_CHANNEL;

static bool mode_chosen = false;
static bool adc_ready 	= false;
static bool tim_ready 	= false;

static float Capacity   = 0;
static float Resistance = 0;
static float Voltage    = 0;
static float Voltage_AC = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) {
		case AC_VOLTAGE_MODE_Pin:
			mode = AC_VOLTAGE_MODE;
			break;
		case DC_VOLTAGE_MODE_RANGE_1_Pin:
			mode = DC_VOLTAGE_MODE_RANGE_1;
			break;
		case DC_VOLTAGE_MODE_RANGE_2_Pin:
			mode = DC_VOLTAGE_MODE_RANGE_2;
			break;
		case DC_VOLTAGE_MODE_RANGE_3_Pin:
			mode = DC_VOLTAGE_MODE_RANGE_3;
			break;
		case RESISTANCE_MODE_RANGE_1_Pin:
			mode = RESISTANCE_MODE_RANGE_1;
			break;
		case RESISTANCE_MODE_RANGE_2_Pin:
			mode = RESISTANCE_MODE_RANGE_2;
			break;
		case RESISTANCE_MODE_RANGE_3_Pin:
			mode = RESISTANCE_MODE_RANGE_3;
			break;
		case CAPACITY_MODE_RANGE_1_Pin:
			mode = CAPACITY_MODE_RANGE_1;
			break;
		case CAPACITY_MODE_RANGE_2_Pin:
			mode = CAPACITY_MODE_RANGE_2;
			break;
		case CAPACITY_MODE_RANGE_3_Pin:
			mode = CAPACITY_MODE_RANGE_3;
			break;
		default:
			Error_Handler();
	}
	mode_chosen = true;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc == &hadc1) {
		if (channel == VOLTAGE_CHANNEL) {
			if (i != 100) {
				adc_V_buf[i] = HAL_ADC_GetValue(hadc);
				i++;
				HAL_ADC_Start_IT(&hadc1);
			}
			channel++;
		}
		else if (channel == RESISTANCE_CHANNEL){
			if (q != 100) {
				adc_R_buf[q] = HAL_ADC_GetValue(hadc);
				q++;
				HAL_ADC_Start_IT(&hadc1);
			}
			channel--;
		}

		if ((q == 100) && (i == 100)) {
			adc_ready = true;
			q = 0;
			i = 0;
		}

	}
	else {
		Error_Handler();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim1)
    {
		freq = TIM2->CNT;

		HAL_TIM_Base_Stop_IT(&htim1);

		tim_ready = true;
    }
	else {
		Error_Handler();
	}
}

static float voltage_channel_average(void)
{
	uint64_t voltage_tmp = 0;

	for (uint8_t j = 0; j < 100; j++) {
		voltage_tmp += adc_V_buf[j];
	}

	return (float)((voltage_tmp * 3.3) / (100 * 4095));
}

static float resistance_channel_average(void)
{
	uint64_t voltage_tmp = 0;

	for (uint8_t j = 0; j < 100; j++) {
		voltage_tmp += adc_R_buf[j];
	}

	return (float)((voltage_tmp * 3.3) / (100 * 4095));
}

static uint32_t voltage_max(void)
{
	uint32_t voltage_tmp = 0;

	for (uint8_t j = 0; j < 100; j++) {
		if (adc_V_buf[j] > voltage_tmp) {
			voltage_tmp = adc_V_buf[j];
		}
	}

	return voltage_tmp;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void default_display_setup(void)
{
	  ssd1306_Init();

	  ssd1306_Fill(White);

	  ssd1306_SetCursor(5,10);
	  ssd1306_WriteString("Mode:", Font_7x10, Black);

	  ssd1306_SetCursor(50,10);
	  ssd1306_WriteChar('|', Font_7x10, Black);

	  ssd1306_SetCursor(60,10);
	  ssd1306_WriteString("Range:", Font_7x10, Black);

	  ssd1306_DrawRectangle(5, 30, 123, 60, Black);

	  ssd1306_UpdateScreen();
}

/* �?змерение переменного напряжения */
static void ac_voltage_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('U', Font_7x10, Black);

		  ssd1306_Line(40, 6, 40, 8, Black);
		  ssd1306_Line(41, 6, 42, 6, Black);
		  ssd1306_Line(43, 7, 44, 7, Black);
		  ssd1306_DrawPixel(44, 6, Black);
		  ssd1306_DrawPixel(44, 5, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_ADC_Start_IT(&hadc1);
	}
	if (adc_ready) {
		Voltage_AC = (float)(voltage_max() * Ku_a * K / 1.4142);
		snprintf(str, sizeof(str), "%f", Voltage_AC);

		ssd1306_SetCursor(10, 37);
		ssd1306_WriteString(str, Font_11x18, Black);
		ssd1306_UpdateScreen();

		adc_ready = false;

		HAL_ADC_Start_IT(&hadc1);
	}
}

/* Измерение постоянного напряжения. Первый диапазон */
static void dc_voltage_1_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('U', Font_7x10, Black);

		  ssd1306_Line(40, 7, 45, 7, Black);

		  ssd1306_SetCursor(100,11);
		  ssd1306_WriteChar('1', Font_7x10, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_ADC_Start_IT(&hadc1);
	}

	if (adc_ready) {
	  float voltage = voltage_channel_average();
	  Voltage = voltage * Ku1 * K;
	  snprintf(str, sizeof(str), "%f", Voltage);

	  ssd1306_SetCursor(10, 37);
	  ssd1306_WriteString(str, Font_11x18, Black);
	  ssd1306_UpdateScreen();

	  adc_ready = false;

	  HAL_ADC_Start_IT(&hadc1);
	}
}

/* �?змерение постоянного напряжения. Второй диапазон */
static void dc_voltage_2_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('U', Font_7x10, Black);

		  ssd1306_Line(40, 7, 45, 7, Black);

		  ssd1306_SetCursor(100,11);
		  ssd1306_WriteChar('2', Font_7x10, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_ADC_Start_IT(&hadc1);
	}

	if (adc_ready) {
	  float voltage = voltage_channel_average();
	  Voltage = voltage * Ku2 * K;
	  snprintf(str, sizeof(str), "%f", Voltage);

	  ssd1306_SetCursor(10, 37);
	  ssd1306_WriteString(str, Font_11x18, Black);
	  ssd1306_UpdateScreen();

	  adc_ready = false;

	  HAL_ADC_Start_IT(&hadc1);
	}
}

/* �?змерение постоянного напряжения. Третий диапазон */
static void dc_voltage_3_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('U', Font_7x10, Black);

		  ssd1306_Line(40, 7, 45, 7, Black);

		  ssd1306_SetCursor(100,11);
		  ssd1306_WriteChar('3', Font_7x10, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_ADC_Start_IT(&hadc1);
	}

	if (adc_ready) {
	  float voltage = voltage_channel_average();
	  Voltage = voltage * Ku3 * K;
	  snprintf(str, sizeof(str), "%f", Voltage);

	  ssd1306_SetCursor(10, 37);
	  ssd1306_WriteString(str, Font_11x18, Black);
	  ssd1306_UpdateScreen();

	  adc_ready = false;

	  HAL_ADC_Start_IT(&hadc1);
	}
}

/* �?змерение сопротивления. Первый диапазон */
static void resistance_1_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('R', Font_7x10, Black);

		  ssd1306_SetCursor(100,11);
		  ssd1306_WriteChar('1', Font_7x10, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_ADC_Start_IT(&hadc1);
	}

	if (adc_ready) {
	  float voltage = resistance_channel_average();
	  Resistance = voltage * Kr1 * K;
	  snprintf(str, sizeof(str), "%f", Resistance);

	  ssd1306_SetCursor(10, 37);
	  ssd1306_WriteString(str, Font_11x18, Black);
	  ssd1306_UpdateScreen();

	  adc_ready = false;

	  HAL_ADC_Start_IT(&hadc1);
	}
}

/* �?змерение сопротивления. Второй диапазон */
static void resistance_2_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('R', Font_7x10, Black);

		  ssd1306_SetCursor(100,11);
		  ssd1306_WriteChar('2', Font_7x10, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_ADC_Start_IT(&hadc1);
	}

	if (adc_ready) {
	  float voltage = resistance_channel_average();
	  Resistance = voltage * Kr2 * K;
	  snprintf(str, sizeof(str), "%f", Resistance);

	  ssd1306_SetCursor(10, 37);
	  ssd1306_WriteString(str, Font_11x18, Black);
	  ssd1306_UpdateScreen();

	  adc_ready = false;

	  HAL_ADC_Start_IT(&hadc1);
	}
}

/* �?змерение сопротивления. Третий диапазон */
static void resistance_3_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('R', Font_7x10, Black);

		  ssd1306_SetCursor(100,11);
		  ssd1306_WriteChar('3', Font_7x10, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_ADC_Start_IT(&hadc1);
	}

	if (adc_ready) {
	  float voltage = resistance_channel_average();
	  Resistance = voltage * Kr3 * K;
	  snprintf(str, sizeof(str), "%f", Resistance);

	  ssd1306_SetCursor(10, 37);
	  ssd1306_WriteString(str, Font_11x18, Black);
	  ssd1306_UpdateScreen();

	  adc_ready = false;

	  HAL_ADC_Start_IT(&hadc1);
	}
}

/* �?змерение емкости. Первый диапазон. P.S. Не уверен, нужны ли другие режимы
 * для измерения емкости, потому что частота измеряется в довольно широком диапазоне.
 * Пока оставил один функциональный режим.
 *  */
static void capacity_1_measure(void)
{
	if (mode_chosen) {
		  ssd1306_SetCursor(40,10);
		  ssd1306_WriteChar('C', Font_7x10, Black);

		  ssd1306_SetCursor(100,11);
		  ssd1306_WriteChar('1', Font_7x10, Black);

		  ssd1306_UpdateScreen();

		  mode_chosen = false;

		  HAL_TIM_Base_Start_IT(&htim1);
		  HAL_TIM_Base_Start(&htim2);
	}

	if (tim_ready) {
        tim_ready = false;

        /*
         * Чтобы получить измеренную частоту, необходимо брать именно freq*2.
         * В таком случае получается частота в Гц.
         * */

        Capacity = (float)((freq * 2) * Kc);
        snprintf(str, sizeof(str), "%f", Capacity);

        ssd1306_SetCursor(10, 37);
        ssd1306_WriteString(str, Font_11x18, Black);
        ssd1306_UpdateScreen();

        __HAL_TIM_SET_COUNTER(&htim2, 0);
        HAL_TIM_Base_Start_IT(&htim1);
	}
}

static void capacity_2_measure(void)
{
//	if (mode_chosen) {
//		  ssd1306_SetCursor(40,10);
//		  ssd1306_WriteChar('C', Font_7x10, Black);
//
//		  ssd1306_SetCursor(100,11);
//		  ssd1306_WriteChar('2', Font_7x10, Black);
//
//		  ssd1306_UpdateScreen();
//
//		  mode_chosen = false;
//
//		  measure_coefficient = 1.0;
//
//		  HAL_TIM_Base_Start_IT(&htim1);
//		  HAL_TIM_Base_Start(&htim2);
//	}
//
//	if (tim_ready) {
//        tim_ready = false;
//
//        char str[50] = {0};
//
//        snprintf(str, sizeof(str), "FREQUENCY: %lu Hz\n--------------------\n", freq * 2);
//        HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), 1000);
//
//        __HAL_TIM_SET_COUNTER(&htim2, 0);
//        HAL_TIM_Base_Start_IT(&htim1);
//	}
}

static void capacity_3_measure(void)
{
//	if (mode_chosen) {
//		  ssd1306_SetCursor(40,10);
//		  ssd1306_WriteChar('C', Font_7x10, Black);
//
//		  ssd1306_SetCursor(100,11);
//		  ssd1306_WriteChar('3', Font_7x10, Black);
//
//		  ssd1306_UpdateScreen();
//
//		  mode_chosen = false;
//
//		  measure_coefficient = 1.0;
//
//		  HAL_TIM_Base_Start_IT(&htim1);
//		  HAL_TIM_Base_Start(&htim2);
//	}
//
//	if (tim_ready) {
//		tim_ready = false;
//
//        char str[50] = {0};
//
//        snprintf(str, sizeof(str), "FREQUENCY: %lu Hz\n--------------------\n", freq * 2);
//        HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), 1000);
//
//        __HAL_TIM_SET_COUNTER(&htim2, 0);
//        HAL_TIM_Base_Start_IT(&htim1);
//	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  default_display_setup();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	  switch(mode) {
	  	  case AC_VOLTAGE_MODE:
	  		  ac_voltage_measure();
	  		  break;
	  	  case DC_VOLTAGE_MODE_RANGE_1:
	  		  dc_voltage_1_measure();
	  		  break;
	  	  case DC_VOLTAGE_MODE_RANGE_2:
	  		  dc_voltage_2_measure();
	  		  break;
	  	  case DC_VOLTAGE_MODE_RANGE_3:
	  		  dc_voltage_3_measure();
	  		  break;
	  	  case RESISTANCE_MODE_RANGE_1:
	  		  resistance_1_measure();
	  		  break;
	  	  case RESISTANCE_MODE_RANGE_2:
	  		  resistance_2_measure();
	  		  break;
	  	  case RESISTANCE_MODE_RANGE_3:
	  		  resistance_3_measure();
	  		  break;
	  	  case CAPACITY_MODE_RANGE_1:
	  		  capacity_1_measure();
	  		  break;
	  	  case CAPACITY_MODE_RANGE_2:
	  		  capacity_2_measure();
	  		  break;
	  	  case CAPACITY_MODE_RANGE_3:
	  		  capacity_3_measure();
	  		  break;
	  	  default:
	  		  Error_Handler();
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
