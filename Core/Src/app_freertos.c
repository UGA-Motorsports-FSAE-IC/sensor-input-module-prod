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
#include "main.h"
#include "stm32-mcp320x-reader/mcp320x.h"
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

  Wheelspeed wheel1;
  Wheelspeed wheel2;
  Wheelspeed wheel3;
  Wheelspeed wheel4;
  uint8_t wsData1[8];  
  uint8_t wsData2[8];
  float milesPerNotch = 1.397222f / 12.0f / 5280.0f;

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

  uint8_t dataforshift [8] = {2, 2, 2, 2, 2, 2, 2, 2};
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
    if (fastcan) {
      fastcan = 0;
      add_message_to_queue(&txheaderWS1, (uint8_t *)wsData1);
      add_message_to_queue(&txheaderWS2, (uint8_t *)wsData2);
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
      wheel1.currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    } else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
      wheel2.currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    } else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
      wheel3.currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
    } else {
      wheel4.currentTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    }
}

/* USER CODE END Application */

