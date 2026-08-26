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
#include "fdcan.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

COM_InitTypeDef BspCOMInit;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void customprint(char * toprint) {
  HAL_UART_Transmit(&huart1, toprint, strlen(toprint), 100);
}

volatile uint8_t sendcan = 0;
volatile uint8_t toggleled = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM14) {
    sendcan = 1;
  }
  if (htim->Instance == TIM15) {
    toggleled = 1;
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
  volatile uint32_t delta;
  volatile uint32_t lastTick;
  volatile uint8_t flag;
} Wheelspeed;

Wheelspeed wheel1;
Wheelspeed wheel2;
Wheelspeed wheel3;
Wheelspeed wheel4;
char wheelspeedData[600] = {0};   

// This is the global callback function that handles ALL EXTI interrupts
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        wheel1.flag = 1;
    }
    if (GPIO_Pin == GPIO_PIN_5) {
        wheel2.flag = 1;
    }
    if (GPIO_Pin == GPIO_PIN_10) {
        wheel3.flag = 1;
    }
    if (GPIO_Pin == GPIO_PIN_11) {
        wheel4.flag = 1;
    }
}

void getWheelSpeed(Wheelspeed * wheel) {
  uint32_t currentTick = HAL_GetTick();
  wheel->delta = currentTick - wheel->lastTick;
  wheel->lastTick = currentTick;
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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  FDCAN_TxHeaderTypeDef txheader1;
  FDCAN_TxHeaderTypeDef txheader2;
  FDCAN_TxHeaderTypeDef txheader3;
  FDCAN_TxHeaderTypeDef txheader4;
  FDCAN_TxHeaderTypeDef txheader5;
  FDCAN_TxHeaderTypeDef txheader6;
  FDCAN_TxHeaderTypeDef txheader7;

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
      add_message_to_queue( &txheader6, (uint8_t *)(adcdata0to7+ 4));
      add_message_to_queue(&txheader5, (uint8_t *)adcdata0to7);

      customprint("sent all sensors\n"); 
    }

    if (wheel1.flag) {
      getWheelSpeed(&wheel1);
      wheel1.flag = 0;
    }
    if (wheel2.flag) {
      getWheelSpeed(&wheel2);
      wheel2.flag = 0;
    }
    if (wheel3.flag) {
      getWheelSpeed(&wheel3);
      wheel3.flag = 0;
    }
    if (wheel4.flag) {
      getWheelSpeed(&wheel4);
      wheel4.flag = 0;
    }
    sprintf(wheelspeedData, "W1: %dlu, W2%lu, W3: %lu, W4: %lu\n", wheel1.delta, wheel2.delta, wheel3.delta, wheel4.delta);
    HAL_UART_Transmit(&huart1, wheelspeedData, strlen(wheelspeedData), 100);

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
