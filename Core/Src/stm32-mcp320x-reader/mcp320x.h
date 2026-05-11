/*
 * mcp3208.h
 *
 *  Created on: May 1, 2025
 *      Author: bhargav
 */

#ifndef SRC_STM32_MCP320X_READER_MCP320X_H_
#define SRC_STM32_MCP320X_READER_MCP320X_H_

#include "spi.h"
#include "gpio.h"

#define ADC_SINGLEENDED 0b00000110
#define ADC_DIFFERENTIAL 0b00000100
#define MCP320X_MAX 4096

typedef enum {
	ERROR_SPI_320X = -1,
	OK_320X = 0
} errors_320X;

typedef enum {
	MCP3204 = 4,
	MCP3208 = 8
} X_version;

typedef struct {
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *port;
	uint16_t pin;
	X_version version;
} MCP320X;

errors_320X read_channel(MCP320X *handle, uint8_t channel, uint16_t *reading);
errors_320X read_all(MCP320X *handle, uint16_t* readings);
void resetADC(MCP320X *handle);


#endif /* SRC_STM32_MCP320X_READER_MCP320X_H_ */
