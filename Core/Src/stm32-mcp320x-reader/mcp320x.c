/*
 * mcp320x.c
 *
 *  Created on: May 1, 2025
 *      Author: bhargav
 */


#include "mcp320x.h"

void beginSPI(MCP320X *handle) {
	HAL_GPIO_WritePin(handle->port, handle->pin, GPIO_PIN_RESET);
}

void endSPI(MCP320X *handle) {
	HAL_GPIO_WritePin(handle->port, handle->pin, GPIO_PIN_SET);
}

void resetADC(MCP320X *handle) {
	beginSPI(handle);
	endSPI(handle);
}

errors_320X read_channel(MCP320X *handle, uint8_t channel, uint16_t *reading) {
	uint8_t txbuffer[3] = {ADC_SINGLEENDED | channel >> 2, channel << 6, 0};
	uint8_t rxbuffer[3];

	beginSPI(handle);
	HAL_StatusTypeDef result = HAL_SPI_TransmitReceive(handle->hspi, txbuffer, rxbuffer, 3, 10);
	endSPI(handle);

	if (result != HAL_OK) {
		return ERROR_SPI_320X;
	} else {
		*reading = ((uint16_t)rxbuffer[2]) | (((uint16_t)rxbuffer[1] & 0b00001111) << 8);
		return OK_320X;
	}
}

errors_320X read_all(MCP320X *handle, uint16_t* readings) {
	for (uint8_t i = 0; i < (handle->version); i++) {
		uint8_t txbuffer[3];
		uint8_t rxbuffer[3];

		txbuffer[0] = ADC_SINGLEENDED | (i >> 2);
		txbuffer[1] = i << 6;
		txbuffer[2] = 0;

		beginSPI(handle);
		HAL_StatusTypeDef result = HAL_SPI_TransmitReceive(handle->hspi, txbuffer, rxbuffer, 3, 10);
		endSPI(handle);

		if (result != HAL_OK) {
			return ERROR_SPI_320X;
		} else {
			readings[i] = ((rxbuffer[1] & 0x0F) << 8) | (rxbuffer[2]);
		}
	}
	return OK_320X;
}




