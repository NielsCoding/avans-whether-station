#include <stm32f0xx_hal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f0xx.h>

#include "esp8266.h"
#include "setup.h"
#include "consts.h"
#include "server.h"
#include "util.h"

// macro for concise sending of multiple commands
#define ws_esp8266_send_seq(cmd) { \
	uint8_t _cmd[] = cmd; \
	ws_server_send(_cmd, sizeof(_cmd)); \
}

uint8_t g_ws_esp8266_dma_rx_buffer[WS_DMA_RX_BUFFER_SIZE];
unsigned int g_ws_esp8266_dma_rx_head = 0;
unsigned int g_ws_esp8266_dma_rx_tail = 0;
uint8_t g_ws_esp8266_dma_tx_buffer[WS_DMA_TX_BUFFER_SIZE];

void DMA1_Ch1_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_usart1_rx); }
void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_usart1_tx); }
void USART1_IRQHandler(void) {
	if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		// https://stackoverflow.com/questions/71039052/hal-uartex-rxeventcallback-circular-dma-what-address-is-the-data
		g_ws_esp8266_dma_rx_head = huart1.RxXferSize - huart1.hdmarx->Instance->CNDTR;

		ws_esp8266_incoming_data_chunk();
	}
	HAL_UART_IRQHandler(&huart1);
}

void ws_esp8266_incoming_data_chunk() {
	if (g_ws_esp8266_dma_rx_head == g_ws_esp8266_dma_rx_tail) return; // no new data
	if (g_ws_esp8266_dma_rx_head > g_ws_esp8266_dma_rx_tail) {
		// read from tail until head
		ws_server_req_incoming(&g_ws_esp8266_dma_rx_buffer[g_ws_esp8266_dma_rx_tail],
			g_ws_esp8266_dma_rx_head - g_ws_esp8266_dma_rx_tail);
	} else /* if (g_ws_esp8266_dma_rx_head < g_ws_esp8266_dma_rx_tail) */ {
		// read from tail until end of buffer
		ws_server_req_incoming(&g_ws_esp8266_dma_rx_buffer[g_ws_esp8266_dma_rx_tail],
			WS_DMA_RX_BUFFER_SIZE - g_ws_esp8266_dma_rx_tail);
		// read from buffer begin until head
		ws_server_req_incoming(&g_ws_esp8266_dma_rx_buffer[0], // yes i know this looks dumb
			g_ws_esp8266_dma_rx_head);
	}
	// finish read by shifting tail forward
	g_ws_esp8266_dma_rx_tail = g_ws_esp8266_dma_rx_head;
}

void ws_esp8266_send(uint8_t* data, size_t size) {
	size_t limited_size = WS_MIN(size, WS_DMA_TX_BUFFER_SIZE - 1);
	memcpy(g_ws_esp8266_dma_tx_buffer, data, limited_size);
	g_ws_esp8266_dma_tx_buffer[limited_size] = 0x00;

#ifdef WS_DBG_PRINT_ESP_OVER_USART2
	ws_dbg_set_usart2_tty_color(WS_DBG_TTY_COLOR_TX);
	HAL_UART_Transmit(&huart2, g_ws_esp8266_dma_tx_buffer, strlen((char*) g_ws_esp8266_dma_tx_buffer), 100);
#endif

	HAL_UART_Transmit_DMA(&huart1, g_ws_esp8266_dma_tx_buffer, strlen((char*) g_ws_esp8266_dma_tx_buffer));
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
}

void ws_esp8266_connect() {
	ws_esp8266_send_seq("AT+CWJAP=\"" WS_ESP8266_WLAN_SSID "\",\"" WS_ESP8266_WLAN_PASSWD "\"\r\n");
}

void ws_esp8266_ap_client_mode() {
	ws_esp8266_send_seq("AT+CWMODE=1\r\n");
}

void ws_esp8266_start_tcp_server() {
	ws_esp8266_send_seq("AT+CIPSERVER=0\r\n"); // stop tcp server (if running)
	ws_esp8266_send_seq("AT+CIPMUX=1\r\n"); // enable multiplexing (allow multiple connections)
	ws_esp8266_send_seq("AT+CIPSERVER=1," WS_SERVER_PORT_STR "\r\n"); // start tcp server
}

void ws_esp8266_set_mac() {
	ws_esp8266_send_seq("AT+CIPSTAMAC=\"" WS_ESP8266_WLAN_MAC "\"\r\n");
}

void ws_esp8266_set_ip() {
	ws_esp8266_send_seq("AT+CIPSTA=\"" WS_ESP8266_WLAN_IP "\"\r\n");
}
