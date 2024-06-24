#include <stm32f0xx_hal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../shared/protocol.h"
#include "esp8266.h"
#include "server.h"
#include "setup.h"
#include "consts.h"
#include "util.h"

ws_s_server_parser g_ws_server_parser = {
	.last_response = WS_SERVER_RC_NONE,
	.mode = WS_SERVER_LM_IDLE,

	.current_channel = 0,
	.channel_data_length = 0,
	.channel_data_counter = 0,
	.channel_listen_mode = WS_SERVER_CL_CHANNEL_ID,

	.rc = { 0 },
};

static ws_s_protocol_req_parser_state* g_ws_protocol_parsers[WS_SERVER_MAX_CHANNELS] = {0};
static unsigned int g_ws_esp8266_dma_tx_buffer_head = 0;
static unsigned int g_ws_esp8266_dma_tx_buffer_tail = 0;
static unsigned int g_ws_esp8266_dma_tx_buffer_cs = 0; // chunk size

void ws_server_req_parse_byte(unsigned int channel, uint8_t byte, bool ignore) {
	if (ignore) return;
	if (channel >= WS_SERVER_MAX_CHANNELS) return;

	if (g_ws_protocol_parsers[channel] == NULL) {
		g_ws_protocol_parsers[channel] = ws_protocol_req_parser_alloc();
	}

	ws_protocol_parse_req_byte(g_ws_protocol_parsers[channel], byte);
}

void ws_server_req_finish(unsigned int channel, bool ignore) {
	if (ignore) return;
	if (channel >= WS_SERVER_MAX_CHANNELS) return;
	
	if (g_ws_protocol_parsers[channel] != NULL) {
		ws_protocol_req_parser_free(g_ws_protocol_parsers[channel]);
		g_ws_protocol_parsers[channel] = NULL;
	}
}

static bool ws_server_is_response(char data, uint8_t* counter, const char* cmd, unsigned short cmd_len) {
	if (data == cmd[*counter]) *counter += 1;
	else *counter = 0;
	if (*counter == cmd_len) return true;
	return false;
}

void ws_server_req_incoming(uint8_t* data, size_t size) {
#ifdef WS_DBG_PRINT_ESP_OVER_USART2
	ws_dbg_set_usart2_tty_color(WS_DBG_TTY_COLOR_RX);
	HAL_UART_Transmit(&huart2, data, size, 100);
#endif

	for (unsigned int i = 0; i < size; i++) {
		uint8_t byte = data[i];

		switch (g_ws_server_parser.mode) {
			case WS_SERVER_LM_CMD_ECHO: {
				if (byte == '\n') g_ws_server_parser.mode = WS_SERVER_LM_STATUS_CODE;
				break;
			}
			case WS_SERVER_LM_STATUS_CODE: {
				bool code_got = false;
				if (ws_server_is_response(byte, &g_ws_server_parser.rc.s_ok, "OK", 2)) {
					code_got = true;
					g_ws_server_parser.last_response = WS_SERVER_RC_OK;
				} else if (ws_server_is_response(byte, &g_ws_server_parser.rc.s_error, "ERROR", 5)) {
					code_got = true;
					g_ws_server_parser.last_response = WS_SERVER_RC_ERR;
				} else if (ws_server_is_response(byte, &g_ws_server_parser.rc.s_fail, "FAIL", 4)) {
					code_got = true;
					g_ws_server_parser.last_response = WS_SERVER_RC_ERR;
				} else if (ws_server_is_response(byte, &g_ws_server_parser.rc.s_busy, "busy p...", 9)) {
					code_got = true;
					g_ws_server_parser.last_response = WS_SERVER_RC_BUSY;
				}
				if (code_got) g_ws_server_parser.mode = WS_SERVER_LM_IDLE;
				break;
			}
			case WS_SERVER_LM_IDLE: {
				if (ws_server_is_response(byte, &g_ws_server_parser.rc.i_ipd, "+IPD,", 5)) {
					g_ws_server_parser.mode = WS_SERVER_LM_IPD_LISTENING;
				} else if (ws_server_is_response(byte, &g_ws_server_parser.rc.i_prompt, ">", 1)) {
					// ^^^ this is ">" on official espressif firmware, "> " on ai-thinker firmware
					g_ws_server_parser.mode = WS_SERVER_LM_CIPSEND_LISTENING;
					ws_server_buffer_send_chunk();
				}
				break;
			}
			case WS_SERVER_LM_IPD_LISTENING: {
				switch (g_ws_server_parser.channel_listen_mode) {
					case WS_SERVER_CL_CHANNEL_ID: {
						if (byte == ',') {
							g_ws_server_parser.channel_listen_mode = WS_SERVER_CL_DATA_LENGTH;
							break;
						}
						g_ws_server_parser.current_channel *= 10;
						g_ws_server_parser.current_channel += byte - '0'; // ascii to int
						break;
					}
					case WS_SERVER_CL_DATA_LENGTH: {
						if (byte == ':') {
							g_ws_server_parser.channel_listen_mode = WS_SERVER_CL_DATA_READ;
							if (g_ws_server_parser.channel_data_length > WS_PROTOCOL_CMD_BUFFER_LEN)
								g_ws_server_parser.channel_data_ignore = true;
							break;
						}
						g_ws_server_parser.channel_data_length *= 10;
						g_ws_server_parser.channel_data_length += byte - '0'; // ascii to int
						break;
					}
					case WS_SERVER_CL_DATA_READ: {
						ws_server_req_parse_byte(g_ws_server_parser.current_channel, byte, g_ws_server_parser.channel_data_ignore);
						g_ws_server_parser.channel_data_counter++;
						if (g_ws_server_parser.channel_data_counter == g_ws_server_parser.channel_data_length) {
							g_ws_server_parser.current_channel = 0;
							g_ws_server_parser.channel_data_counter = 0;
							g_ws_server_parser.channel_data_length = 0;
							g_ws_server_parser.channel_listen_mode = WS_SERVER_CL_CHANNEL_ID;
							ws_server_req_finish(g_ws_server_parser.current_channel, g_ws_server_parser.channel_data_ignore);
							g_ws_server_parser.mode = WS_SERVER_LM_IDLE;
							ws_server_buffer_request_chunk_send();
						}
						break;
					}
					default: {}
				}
				break;
			}
			case WS_SERVER_LM_CIPSEND_LISTENING: {
				if (ws_server_is_response(byte, &g_ws_server_parser.rc.l_send_ok, "SEND OK", 7) || ws_server_is_response(byte, &g_ws_server_parser.rc.l_error, "ERROR", 5)) {
					ws_server_buffer_request_chunk_send();
				}
				break;
			}
			default: {}
		}
	}
}

/** @brief get amount of bytes in g_ws_esp8266_dma_tx_buffer until \n */
static unsigned int ws_server_next_line_length() {
	for (unsigned int i = g_ws_esp8266_dma_tx_buffer_tail; i <= g_ws_esp8266_dma_tx_buffer_head; i++)
		if (g_ws_esp8266_dma_tx_buffer[i] == '\n') return i - g_ws_esp8266_dma_tx_buffer_tail + 1;
	return g_ws_esp8266_dma_tx_buffer_head - g_ws_esp8266_dma_tx_buffer_tail;
}

void ws_server_send(uint8_t* data, size_t size) {
	g_ws_server_parser.mode = WS_SERVER_LM_CMD_ECHO;
	ws_esp8266_send(data, size);
	while (g_ws_server_parser.mode != WS_SERVER_LM_IDLE) {};
}

void ws_server_buffer_send_append(uint8_t* data, size_t size) {
  size_t limited_size = WS_MIN(size, WS_DMA_TX_BUFFER_SIZE - g_ws_esp8266_dma_tx_buffer_head);
	strncpy((char*) &g_ws_esp8266_dma_tx_buffer[g_ws_esp8266_dma_tx_buffer_head], (char*) data, limited_size); // append string
	g_ws_esp8266_dma_tx_buffer_head += size; // shift head
}

void ws_server_buffer_request_chunk_send() {
	g_ws_esp8266_dma_tx_buffer_cs = ws_server_next_line_length();

	char* cmd = NULL;
	size_t len;

	if (g_ws_esp8266_dma_tx_buffer_cs > 0) {
		len = asiprintf(&cmd, "AT+CIPSEND=%d,%d\r\n", g_ws_server_parser.current_channel, g_ws_esp8266_dma_tx_buffer_cs);
	} else {
		len = asiprintf(&cmd, "AT+CIPCLOSE=%d\r\n", g_ws_server_parser.current_channel);
	}
	free(cmd);

	g_ws_server_parser.mode = WS_SERVER_LM_CMD_ECHO;

#ifdef WS_DBG_PRINT_ESP_OVER_USART2
	ws_dbg_set_usart2_tty_color(WS_DBG_TTY_COLOR_TX);
	HAL_UART_Transmit(&huart2, (uint8_t*) cmd, len, 100);
#endif
	HAL_UART_Transmit(&huart1, (uint8_t*) cmd, len, 100);
}

void ws_server_buffer_send_chunk() {
#ifdef WS_DBG_PRINT_ESP_OVER_USART2
	ws_dbg_set_usart2_tty_color(WS_DBG_TTY_COLOR_TX);
 	HAL_UART_Transmit(&huart2, &g_ws_esp8266_dma_tx_buffer[g_ws_esp8266_dma_tx_buffer_tail], g_ws_esp8266_dma_tx_buffer_cs, 100);
#endif
 	HAL_UART_Transmit(&huart1, &g_ws_esp8266_dma_tx_buffer[g_ws_esp8266_dma_tx_buffer_tail], g_ws_esp8266_dma_tx_buffer_cs, 100);
	g_ws_esp8266_dma_tx_buffer_tail += g_ws_esp8266_dma_tx_buffer_cs;

	if (g_ws_esp8266_dma_tx_buffer_head == g_ws_esp8266_dma_tx_buffer_tail) {
		g_ws_esp8266_dma_tx_buffer_head = g_ws_esp8266_dma_tx_buffer_tail = 0;
	}
}

