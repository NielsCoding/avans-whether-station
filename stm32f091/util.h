#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stm32f0xx_hal.h>
#include <string.h>

#include "setup.h"
#include "../shared/util.h"

#define ws_usb_printf(fmt, ...) { \
	char temp[255]; \
	snprintf(temp, 255, fmt, ##__VA_ARGS__); \
	HAL_UART_Transmit(&huart2, (uint8_t*) temp, sizeof(char) * strlen(temp), HAL_MAX_DELAY); \
}

