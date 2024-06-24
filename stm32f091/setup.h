#pragma once

#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_i2c.h>
#include <stm32f0xx_hal_uart.h>

#include "consts.h"

/** @brief glbal HAL i2c-1 state */
extern I2C_HandleTypeDef hi2c1;
/** @brief glbal HAL uart-1 state */
extern UART_HandleTypeDef huart1;
/** @brief glbal HAL uart-1 state */
extern UART_HandleTypeDef huart2;
/** @brief glbal HAL DMA state for uart-1's RX channel */
extern DMA_HandleTypeDef hdma_usart1_rx;
/** @brief glbal HAL DMA state for uart-1's TX channel */
extern DMA_HandleTypeDef hdma_usart1_tx;

/** @brief "main" setup function */
void ws_io_setup();

// required HAL setup functions
void NMI_Handler();
void HardFault_Handler();
void SysTick_Handler();
void HAL_MspInit();
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);

