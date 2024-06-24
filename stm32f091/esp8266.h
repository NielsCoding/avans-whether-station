#pragma once

#include <stm32f0xx_hal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "consts.h"

/** @brief DMA rx buffer */
extern uint8_t g_ws_esp8266_dma_rx_buffer[WS_DMA_RX_BUFFER_SIZE];
/** @brief null-terminated tx buffer string */
extern uint8_t g_ws_esp8266_dma_tx_buffer[WS_DMA_TX_BUFFER_SIZE];

/** @brief DMA1 channel 2-3 & DMA2 channel 1-2 interrupt handler */
void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void);
/** @brief DMA1 channel 1 interrupt handler */
void DMA1_Ch1_IRQHandler(void);
/** @brief USART1 interrupt handler */
void USART1_IRQHandler(void);

/** @brief send data to esp over uart with dma */
void ws_esp8266_send(uint8_t* data, size_t size);

/** @brief line idle, handle new data on dma buffer */
void ws_esp8266_incoming_data_chunk();

/** @brief connect to access point using wifi.h credentials */
void ws_esp8266_connect();
/** @brief set esp to access point client mode (connect to AP, not become one) */
void ws_esp8266_ap_client_mode();
/** @brief initialize and configure the tcp server */
void ws_esp8266_start_tcp_server();

/** @brief set mac address of the esp client */
void ws_esp8266_set_mac();
/** @brief set static ip address of the esp client */
void ws_esp8266_set_ip();
