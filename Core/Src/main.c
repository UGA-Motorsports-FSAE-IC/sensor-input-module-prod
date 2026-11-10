/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os2.h"
#include "fdcan.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32c0xx_hal_gpio.h"
#include <stdint.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
FDCAN_FilterTypeDef canfilter;
FDCAN_TxHeaderTypeDef txheader1;
FDCAN_TxHeaderTypeDef txheader2;
FDCAN_TxHeaderTypeDef txheader3;
FDCAN_TxHeaderTypeDef txheader4;
FDCAN_TxHeaderTypeDef txheader5;
FDCAN_TxHeaderTypeDef txheader6;
FDCAN_TxHeaderTypeDef txheader7;
FDCAN_TxHeaderTypeDef txheaderWS1;
FDCAN_TxHeaderTypeDef txheaderWS2;
FDCAN_TxHeaderTypeDef txheader9;
FDCAN_TxHeaderTypeDef txheader10;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_FDCAN1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM14_Init();
  MX_IWDG_Init();
  MX_TIM17_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  txheader1.Identifier = 210;
  txheader1.IdType = FDCAN_STANDARD_ID;
  txheader1.TxFrameType = FDCAN_DATA_FRAME;
  txheader1.DataLength = FDCAN_DLC_BYTES_8;
  txheader1.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader1.BitRateSwitch = FDCAN_BRS_OFF;
  txheader1.FDFormat = FDCAN_CLASSIC_CAN;
  txheader1.MessageMarker = 0;

  txheader2.Identifier = 211;
  txheader2.IdType = FDCAN_STANDARD_ID;
  txheader2.TxFrameType = FDCAN_DATA_FRAME;
  txheader2.DataLength = FDCAN_DLC_BYTES_8;
  txheader2.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader2.BitRateSwitch = FDCAN_BRS_OFF;
  txheader2.FDFormat = FDCAN_CLASSIC_CAN;
  txheader2.MessageMarker = 0;

  txheader3.Identifier = 212;
  txheader3.IdType = FDCAN_STANDARD_ID;
  txheader3.TxFrameType = FDCAN_DATA_FRAME;
  txheader3.DataLength = FDCAN_DLC_BYTES_8;
  txheader3.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader3.BitRateSwitch = FDCAN_BRS_OFF;
  txheader3.FDFormat = FDCAN_CLASSIC_CAN;
  txheader3.MessageMarker = 0;

  txheader4.Identifier = 213;
  txheader4.IdType = FDCAN_STANDARD_ID;
  txheader4.TxFrameType = FDCAN_DATA_FRAME;
  txheader4.DataLength = FDCAN_DLC_BYTES_8;
  txheader4.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader4.BitRateSwitch = FDCAN_BRS_OFF;
  txheader4.FDFormat = FDCAN_CLASSIC_CAN;
  txheader4.MessageMarker = 0;

  txheader5.Identifier = 214;
  txheader5.IdType = FDCAN_STANDARD_ID;
  txheader5.TxFrameType = FDCAN_DATA_FRAME;
  txheader5.DataLength = FDCAN_DLC_BYTES_8;
  txheader5.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader5.BitRateSwitch = FDCAN_BRS_OFF;
  txheader5.FDFormat = FDCAN_CLASSIC_CAN;
  txheader5.MessageMarker = 0;

  txheader6.Identifier = 215;
  txheader6.IdType = FDCAN_STANDARD_ID;
  txheader6.TxFrameType = FDCAN_DATA_FRAME;
  txheader6.DataLength = FDCAN_DLC_BYTES_8;
  txheader6.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader6.BitRateSwitch = FDCAN_BRS_OFF;
  txheader6.FDFormat = FDCAN_CLASSIC_CAN;
  txheader6.MessageMarker = 0;

  txheader7.Identifier = 10;
  txheader7.IdType = FDCAN_STANDARD_ID;
  txheader7.TxFrameType = FDCAN_DATA_FRAME;
  txheader7.DataLength = FDCAN_DLC_BYTES_8;
  txheader7.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader7.BitRateSwitch = FDCAN_BRS_OFF;
  txheader7.FDFormat = FDCAN_CLASSIC_CAN;
  txheader7.MessageMarker = 0;

  txheaderWS1.Identifier = 190;
  txheaderWS1.IdType = FDCAN_STANDARD_ID;
  txheaderWS1.TxFrameType = FDCAN_DATA_FRAME;
  txheaderWS1.DataLength = FDCAN_DLC_BYTES_8;
  txheaderWS1.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheaderWS1.BitRateSwitch = FDCAN_BRS_OFF;
  txheaderWS1.FDFormat = FDCAN_CLASSIC_CAN;
  txheaderWS1.MessageMarker = 0;

  txheaderWS2.Identifier = 191;
  txheaderWS2.IdType = FDCAN_STANDARD_ID;
  txheaderWS2.TxFrameType = FDCAN_DATA_FRAME;
  txheaderWS2.DataLength = FDCAN_DLC_BYTES_8;
  txheaderWS2.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheaderWS2.BitRateSwitch = FDCAN_BRS_OFF;
  txheaderWS2.FDFormat = FDCAN_CLASSIC_CAN;
  txheaderWS2.MessageMarker = 0;

  txheader9.Identifier = 0x140810FF;
  txheader9.IdType = FDCAN_EXTENDED_ID;
  txheader9.TxFrameType = FDCAN_DATA_FRAME;
  txheader9.DataLength = FDCAN_DLC_BYTES_8;
  txheader9.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader9.BitRateSwitch = FDCAN_BRS_OFF;
  txheader9.FDFormat = FDCAN_CLASSIC_CAN;
  txheader9.MessageMarker = 0;

  txheader10.Identifier = 217;
  txheader10.IdType = FDCAN_EXTENDED_ID;
  txheader10.TxFrameType = FDCAN_DATA_FRAME;
  txheader10.DataLength = FDCAN_DLC_BYTES_8;
  txheader10.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader10.BitRateSwitch = FDCAN_BRS_OFF;
  txheader10.FDFormat = FDCAN_CLASSIC_CAN;
  txheader10.MessageMarker = 0;

  canfilter.IdType = FDCAN_STANDARD_ID;
  canfilter.FilterIndex = 0;
  canfilter.FilterType = FDCAN_FILTER_MASK;
  canfilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  canfilter.FilterID1 = 0x000;
  canfilter.FilterID2 = 0x000;

  HAL_FDCAN_ConfigFilter(&hfdcan1, &canfilter);

  canfilter.IdType = FDCAN_EXTENDED_ID;
  canfilter.FilterIndex = 0;
  canfilter.FilterID1 = 0x02400000;
  canfilter.FilterID2 = 0x1FFFFFFF;
  
  HAL_FDCAN_ConfigFilter(&hfdcan1, &canfilter);

  HAL_FDCAN_Start(&hfdcan1);

  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  HAL_TIM_Base_Start_IT(&htim14); 
  HAL_TIM_Base_Start_IT(&htim17);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Call init function for freertos objects (in app_freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
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

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
#ifdef USE_FULL_ASSERT
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
