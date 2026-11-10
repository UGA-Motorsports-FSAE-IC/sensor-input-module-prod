/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os2.h"
#include "main.h"
#include "stm32-mcp320x-reader/mcp320x.h"
#include "stm32c0xx_hal_gpio.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

  extern FDCAN_HandleTypeDef hfdcan1;
  extern IWDG_HandleTypeDef hiwdg;
  extern SPI_HandleTypeDef hspi1; 

  volatile uint8_t sendcan = 0;
  volatile uint8_t toggleled = 0;
  volatile uint8_t fastcan = 0;

  FDCAN_RxHeaderTypeDef rxHeader;
  uint8_t rxData[8];
  uint32_t id;
  volatile uint8_t dataRecieved = 0;

  Wheelspeed wheels[4];
  uint8_t wsData1[8];  
  uint8_t wsData2[8];
  float milesPerNotch = 1.397222f / 12.0f / 5280.0f;

  extern FDCAN_TxHeaderTypeDef txheader1;
  extern FDCAN_TxHeaderTypeDef txheader2;
  extern FDCAN_TxHeaderTypeDef txheader3;
  extern FDCAN_TxHeaderTypeDef txheader4;
  extern FDCAN_TxHeaderTypeDef txheader5;
  extern FDCAN_TxHeaderTypeDef txheader6;
  extern FDCAN_TxHeaderTypeDef txheader7;
  extern FDCAN_TxHeaderTypeDef txheaderWS1;
  extern FDCAN_TxHeaderTypeDef txheaderWS2;
  extern FDCAN_TxHeaderTypeDef txheader9;
  extern FDCAN_TxHeaderTypeDef txheader10;


/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for CANTask */
osThreadId_t CANTaskHandle;
const osThreadAttr_t CANTask_attributes = {
  .name = "CANTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 4
};
/* Definitions for WSTask */
osThreadId_t WSTaskHandle;
const osThreadAttr_t WSTask_attributes = {
  .name = "WSTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM14) {
    sendcan = 1;
  }
  if (htim->Instance == TIM17) {
    fastcan = 1;
  }
  if (htim->Instance == TIM3)
  {
    HAL_IncTick();
  }
}

void add_message_to_queue(FDCAN_TxHeaderTypeDef * header, uint8_t * datatosend) {
  while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) == 0) {
    continue;
  }
  
  HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, header, (const uint8_t *)datatosend);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {

    HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxData);

    id = rxHeader.Identifier;
    dataRecieved = 1;
  }
}
/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of CANTask */
  CANTaskHandle = osThreadNew(StartCanTask, NULL, &CANTask_attributes);

  /* creation of WSTask */
  WSTaskHandle = osThreadNew(StartWSTask, NULL, &WSTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */

  /* Infinite loop */
  for(;;)
  {
    osDelay(500);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_StartCanTask */
/**
* @brief Function implementing the CANTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCanTask */
void StartCanTask(void *argument)
{
  /* USER CODE BEGIN CANTask */
  MCP320X adc8to15 = { &hspi1, GPIOB, GPIO_PIN_0, MCP3208 };
  MCP320X adc0to7 = { &hspi1, GPIOB, GPIO_PIN_1, MCP3208 };
  MCP320X adc16to23 = { &hspi1, GPIOB, GPIO_PIN_2, MCP3208 };

  resetADC(&adc8to15);
  resetADC(&adc0to7);
  resetADC(&adc16to23);
  uint16_t adcdata8to15[8];
  uint16_t adcdata0to7[8];
  uint16_t adcdata16to23[8]; 

  uint8_t egtData[8];
  uint8_t heartBeatData[8];
  heartBeatData[2] = 0x0C;
  heartBeatData[4] = 0x84;
  
  /* Infinite loop */
  for(;;)
  {
    if (sendcan) {
      sendcan = 0;
      
      HAL_IWDG_Refresh(&hiwdg);

      read_all(&adc8to15, adcdata8to15);
      add_message_to_queue(&txheader1, (uint8_t *)adcdata8to15);
      add_message_to_queue(&txheader2, (uint8_t *)(adcdata8to15 + 4));
      
      read_all(&adc16to23, adcdata16to23);
      add_message_to_queue(&txheader3, (uint8_t *)adcdata16to23);
      add_message_to_queue(&txheader4, (uint8_t *)(adcdata16to23 + 4));
      
      read_all(&adc0to7, adcdata0to7);
      add_message_to_queue(&txheader6, (uint8_t *)(adcdata0to7+ 4));
      add_message_to_queue(&txheader5, (uint8_t *)adcdata0to7);

      add_message_to_queue(&txheader10, (uint8_t *)egtData); //EGT values
      add_message_to_queue(&txheader9, (uint8_t *)heartBeatData); //EGT heartbeat

    }
    if (dataRecieved) {
      if (id == 0x02400000) {
        for (uint8_t i = 0; i < 7; i+=2) {
          float num = (0.225f * (((uint16_t)rxData[i] << 8) | rxData[i+1])) + 32; 
          egtData[i] = (uint8_t)num;
          egtData[i+1] = (uint8_t)((num - (uint8_t)num) * 100);
        }
      }
    }
  }
  /* USER CODE END CANTask */
}

/* USER CODE BEGIN Header_StartWSTask */
/**
* @brief Function implementing the WSTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWSTask */
void StartWSTask(void *argument)
{
  /* USER CODE BEGIN WSTask */

  /* Infinite loop */
  for(;;)
  {
    for (int i = 0; i < 4; i++) {
      getWheelSpeed(&wheels[i]);
      if (i < 2) {
        memcpy(&wsData1[i * sizeof(float)], &wheels[i].speed, sizeof(float));
      } else {
        memcpy(&wsData2[i * sizeof(float)], &wheels[i].speed, sizeof(float));
      }
    }
    if (fastcan) {
      fastcan = 0;
      add_message_to_queue(&txheaderWS1, (uint8_t *)wsData1);
      add_message_to_queue(&txheaderWS2, (uint8_t *)wsData2);
    }
  }
  /* USER CODE END WSTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

// Gets delta between last notch scene, and averages between the last three deltas.
void getWheelSpeed(Wheelspeed * wheel) {
  //wheel->lastDelta = wheel->midDelta;
  //wheel->midDelta = wheel->delta;
  /*
  float average = (wheel->lastDelta + wheel->midDelta + wheel->delta) / 3.0f;
  if (average == 0) {
    return;
  }
  */
  //float averageInSeconds = average / 1000.0f;
  wheel->delta = wheel->currentTick - wheel->lastTick;
  wheel->lastTick = wheel->currentTick;
  wheel->speed = ((milesPerNotch / (wheel->delta / 1000000.0f)) * 3600.0f);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
      wheels[1].currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    } else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
      wheels[2].currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    } else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
      wheels[3].currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
    } else {
      wheels[4].currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    }
}

/* USER CODE END Application */

