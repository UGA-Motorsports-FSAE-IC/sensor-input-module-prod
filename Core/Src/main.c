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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

FDCAN_HandleTypeDef hfdcan1;

IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM14_Init(void);
static void MX_TIM15_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM17_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void customprint(char * toprint) {
  HAL_UART_Transmit(&huart1, toprint, strlen(toprint), 100);
}

volatile uint8_t sendcan = 0;
volatile uint8_t toggleled = 0;
volatile uint8_t fastcan = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM14) {
    sendcan = 1;
  }
  if (htim->Instance == TIM15) {
    toggleled = 1;
  }
  if (htim->Instance == TIM17) {
    fastcan = 1;
  }
}

void add_message_to_queue(FDCAN_TxHeaderTypeDef * header, uint8_t * datatosend) {
  while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) == 0) {
    continue;
  }
  
  uint32_t errormessage = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, header, (const uint8_t *)datatosend);
  if (errormessage != HAL_OK) {
    char buffer[40] = {0};
    sprintf(buffer, "message send fail, error code %lu", errormessage);
    customprint(buffer);
  }
}

uint8_t onebyte;
volatile uint8_t gotcommand = 0;
volatile int fulllen = 40;
char intbuffer[40] = {0};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  intbuffer[40 - fulllen] = onebyte;
  if (onebyte == '!') {
    intbuffer[40 - fulllen] = '\0';
    gotcommand = 1;
    return;
  } else {
    fulllen--;
  }
  
  HAL_UART_Receive_IT(&huart1, &onebyte, 1);
}


char buffer[40] = {0};

int getcommand(uint8_t * buffer, int maxlen) {
  memset(buffer, 0, maxlen);
  uint8_t singlebyte = 0;
  while (maxlen) {
    HAL_UART_Receive(&huart1, &singlebyte, 1, 0xFFFF);
    buffer[40 - maxlen] = singlebyte;

    if (singlebyte == '!') {
      buffer[40 - maxlen] = '\0';
      break;
    }

    maxlen--;
  }

  return maxlen;
}

typedef struct {
  volatile float speed;
  volatile uint32_t delta;
  volatile uint32_t lastDelta;
  volatile uint32_t midDelta;
  volatile uint32_t lastTick;
  volatile uint8_t flag;
} Wheelspeed;

Wheelspeed wheel1;
Wheelspeed wheel2;
Wheelspeed wheel3;
Wheelspeed wheel4;
uint8_t wsData[8];  
float milesPerNotch = 1.397222f / 12.0f / 5280.0f;

// This is the global callback function that handles ALL EXTI interrupts
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == WS1_Pin) {
        wheel1.flag = 1;
    }
    if (GPIO_Pin == WS2_Pin) {
        wheel2.flag = 1;
    }
    if (GPIO_Pin == WS3_Pin) {
        wheel3.flag = 1;
    }
    if (GPIO_Pin == WS4_Pin) {
        wheel4.flag = 1;
    }
}

// Gets delta between last notch scene, and averages between the last three deltas.
void getWheelSpeed(Wheelspeed * wheel) {
  //wheel->lastDelta = wheel->midDelta;
  //wheel->midDelta = wheel->delta;

  uint32_t currentTick = HAL_GetTick();
  wheel->delta = currentTick - wheel->lastTick;
  wheel->lastTick = currentTick;
  /*
  float average = (wheel->lastDelta + wheel->midDelta + wheel->delta) / 3.0f;
  if (average == 0) {
    return;
  }
  */
  //float averageInSeconds = average / 1000.0f;
  wheel->speed = ((milesPerNotch / (wheel->delta / 1000.0f)) * 3600.0f);
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
  MX_FDCAN1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM14_Init();
  MX_TIM15_Init();
  MX_IWDG_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  FDCAN_TxHeaderTypeDef txheader1;
  FDCAN_TxHeaderTypeDef txheader2;
  FDCAN_TxHeaderTypeDef txheader3;
  FDCAN_TxHeaderTypeDef txheader4;
  FDCAN_TxHeaderTypeDef txheader5;
  FDCAN_TxHeaderTypeDef txheader6;
  FDCAN_TxHeaderTypeDef txheader7;
  FDCAN_TxHeaderTypeDef txheader8;

  FDCAN_FilterTypeDef canfilter;

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

  txheader8.Identifier = 190;
  txheader8.IdType = FDCAN_STANDARD_ID;
  txheader8.TxFrameType = FDCAN_DATA_FRAME;
  txheader8.DataLength = FDCAN_DLC_BYTES_8;
  txheader8.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txheader8.BitRateSwitch = FDCAN_BRS_OFF;
  txheader8.FDFormat = FDCAN_CLASSIC_CAN;
  txheader8.MessageMarker = 0;
  
  canfilter.IdType = FDCAN_STANDARD_ID;
  canfilter.FilterIndex = 0;
  canfilter.FilterType = FDCAN_FILTER_MASK;
  canfilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  canfilter.FilterID1 = 0x000;
  canfilter.FilterID2 = 0x000;
  
  //HAL_UART_Receive_IT(&huart1, &onebyte, 1)
  

  HAL_FDCAN_ConfigFilter(&hfdcan1, &canfilter);

  HAL_FDCAN_Start(&hfdcan1);

  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  HAL_TIM_Base_Start_IT(&htim14); 
  HAL_TIM_Base_Start_IT(&htim15); 
  HAL_TIM_Base_Start_IT(&htim17);


  MCP320X adc8to15 = {
    &hspi1,
    GPIOB,
    GPIO_PIN_0,
    MCP3208
  };

  MCP320X adc0to7 = {
    &hspi1,
    GPIOB,
    GPIO_PIN_1,
    MCP3208
  };

  MCP320X adc16to23 = {
    &hspi1,
    GPIOB,
    GPIO_PIN_2,
    MCP3208
  };

  resetADC(&adc8to15);
  resetADC(&adc0to7);
  resetADC(&adc16to23);
  uint16_t adcdata8to15[8];
  uint16_t adcdata0to7[8];
  uint16_t adcdata16to23[8]; 

  while (1)
  {

    if (toggleled) {
      toggleled = 0;       
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
    }

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

      customprint("sent all sensors\n"); 
    }

    if (fastcan) {
      fastcan = 0;
      add_message_to_queue(&txheader8, (uint8_t *)wsData);
    }

    if (wheel1.flag) {
      getWheelSpeed(&wheel1);
      wsData[0] = (uint8_t) wheel1.speed;
      wsData[1] = (uint8_t)((wheel1.speed - (uint8_t)wheel1.speed) * 100);
      wheel1.flag = 0;
    }
    if (wheel2.flag) {
      getWheelSpeed(&wheel2);
      wsData[2] = (uint8_t) wheel2.speed;
      wsData[3] = (uint8_t)((wheel2.speed - (uint8_t)wheel2.speed) * 100);
      wheel2.flag = 0;
    }
    HAL_IWDG_Refresh(&hiwdg);
    if (wheel3.flag) {
      getWheelSpeed(&wheel3);
      wsData[4] = (uint8_t) wheel3.speed;
      wsData[5] = (uint8_t)((wheel3.speed - (uint8_t)wheel3.speed) * 100);

      wheel3.flag = 0;
    }
    if (wheel4.flag) {
      getWheelSpeed(&wheel4);
      wsData[6] = (uint8_t) wheel4.speed;
      wsData[7] = (uint8_t)((wheel4.speed - (uint8_t)wheel4.speed) * 100);

      wheel4.flag = 0;
    }
    //sprintf(wheelspeedData, "W1: %dlu, W2%lu, W3: %lu, W4: %lu\n", wheel1.delta, wheel2.delta, wheel3.delta, wheel4.delta);
    //HAL_UART_Transmit(&huart1, wheelspeedData, strlen(wheelspeedData), 100);

      /*
      char finalbuffer[600] = {0};      
      sprintf(finalbuffer, "%u, %u, %u, %u, %u, %u, %u, %u\n", adcdata8to15[0], adcdata8to15[1], adcdata8to15[2], adcdata8to15[3], adcdata8to15[4], adcdata8to15[5], adcdata8to15[6], adcdata8to15[7]);
      HAL_UART_Transmit(&huart1, finalbuffer, strlen(finalbuffer), 100);
      */


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

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 2;
  hfdcan1.Init.NominalTimeSeg1 = 9;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 5;
  hfdcan1.Init.DataTimeSeg1 = 6;
  hfdcan1.Init.DataTimeSeg2 = 5;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 4;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 65535;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 40;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 65535;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 11;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 49999;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : WS1_Pin */
  GPIO_InitStruct.Pin = WS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(WS1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : WS2_Pin */
  GPIO_InitStruct.Pin = WS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(WS2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : WS3_Pin WS4_Pin */
  GPIO_InitStruct.Pin = WS3_Pin|WS4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_TIM1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
