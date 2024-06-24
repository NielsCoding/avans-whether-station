#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stm32f0xx_hal.h>
#include <string.h>

#include "setup.h"
#include "../shared/util.h"

// macro to string macro's
#define WS_MTS_EXPAND(x) #x
#define WS_MTS(x) WS_MTS_EXPAND(x)

#define WS_DBG_TTY_COLOR_BLK    0x0
#define WS_DBG_TTY_COLOR_RED    0x1
#define WS_DBG_TTY_COLOR_GRN    0x2
#define WS_DBG_TTY_COLOR_YLW    0x3
#define WS_DBG_TTY_COLOR_BLU    0x4
#define WS_DBG_TTY_COLOR_VLT    0x5
#define WS_DBG_TTY_COLOR_CYN    0x6
#define WS_DBG_TTY_COLOR_WHI    0x7
#define WS_DBG_TTY_COLOR_BR_BLK 0x8
#define WS_DBG_TTY_COLOR_BR_RED 0x9
#define WS_DBG_TTY_COLOR_BR_GRN 0xa
#define WS_DBG_TTY_COLOR_BR_YLW 0xb
#define WS_DBG_TTY_COLOR_BR_BLU 0xc
#define WS_DBG_TTY_COLOR_BR_VLT 0xd
#define WS_DBG_TTY_COLOR_BR_CYN 0xe
#define WS_DBG_TTY_COLOR_BR_WHI 0xf

// serial debug color scheme
#define WS_DBG_TTY_COLOR_DBGMSG WS_DBG_TTY_COLOR_BR_BLK
#define WS_DBG_TTY_COLOR_TX WS_DBG_TTY_COLOR_GRN
#define WS_DBG_TTY_COLOR_RX WS_DBG_TTY_COLOR_RED

// allow `debugger;` statement like in JS
#define debugger asm("nop")

// unused
// #define WS_DBG_TTY_COLOR_CMD_ECHO WS_DBG_TTY_COLOR_BLK
// #define WS_DBG_TTY_COLOR_STATUS_CODE WS_DBG_TTY_COLOR_YLW
// #define WS_DBG_TTY_COLOR_IDLE WS_DBG_TTY_COLOR_VLT
// #define WS_DBG_TTY_COLOR_IPD_LISTENING WS_DBG_TTY_COLOR_GRN
// #define WS_DBG_TTY_COLOR_CIPSEND_LISTENING WS_DBG_TTY_COLOR_RED

/** @brief `printf()` over uart2 (usb) */
#define ws_usb_printf(fmt, ...) { \
	char temp[255]; \
	snprintf(temp, 255, fmt, ##__VA_ARGS__); \
	HAL_UART_Transmit(&huart2, (uint8_t*) temp, sizeof(char) * strlen(temp), HAL_MAX_DELAY); \
}

/** @brief print VT100 escape code over uart2 to set tty color */
#define ws_dbg_set_usart2_tty_color(color) { \
	uint8_t sgr[] = { 0x1b, 0x5b, 0x33 + (color > 7 ? 6 : 0), 0x30 + (color & 0b111), 0x6d }; \
	HAL_UART_Transmit(&huart2, sgr, sizeof(sgr), 100); \
}
