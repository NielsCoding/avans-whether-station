#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_rcc.h>
#include <stm32f0xx_hal_i2c.h>
#include <stm32f0xx_hal_uart.h>
#include <FreeRTOS.h>
#include <task.h>

#include "esp8266.h"
#include "setup.h"
#include "backlog.h"
#include "server.h"
#include "util.h"

I2C_HandleTypeDef hi2c1 = {
	.Instance = I2C1,
	.Init.Timing = 0x2000090E,
	.Init.OwnAddress1 = 0,
	.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT,
	.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE,
	.Init.OwnAddress2 = 0,
	.Init.OwnAddress2Masks = I2C_OA2_NOMASK,
	.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE,
	.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE,
};

UART_HandleTypeDef huart1 = {
	.Instance = USART1,
	.Init.BaudRate = 115200,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16,
};

UART_HandleTypeDef huart2 = {
	.Instance = USART2,
	.Init.BaudRate = 115200,
	.Init.WordLength = UART_WORDLENGTH_8B,
	.Init.StopBits = UART_STOPBITS_1,
	.Init.Parity = UART_PARITY_NONE,
	.Init.Mode = UART_MODE_TX_RX,
	.Init.HwFlowCtl = UART_HWCONTROL_NONE,
	.Init.OverSampling = UART_OVERSAMPLING_16,
	.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE,
	.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT,
};

DMA_HandleTypeDef hdma_usart1_rx = {
	.Instance = DMA1_Channel1,
	.Init.Direction = DMA_PERIPH_TO_MEMORY,
	.Init.PeriphInc = DMA_PINC_DISABLE,
	.Init.MemInc = DMA_MINC_ENABLE,
	.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
	.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE,
	.Init.Mode = DMA_CIRCULAR,
	.Init.Priority = DMA_PRIORITY_LOW,
};

DMA_HandleTypeDef hdma_usart1_tx = {
	.Instance = DMA1_Channel2,
	.Init.Direction = DMA_MEMORY_TO_PERIPH,
	.Init.PeriphInc = DMA_PINC_DISABLE,
	.Init.MemInc = DMA_MINC_ENABLE,
	.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
	.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE,
	.Init.Mode = DMA_NORMAL,
	.Init.Priority = DMA_PRIORITY_LOW,
};

static void ws_io_clock_setup();
static void ws_io_i2c_setup();
static void ws_io_usart1_setup();
static void ws_io_usart2_setup();
static void ws_io_dma_setup();
static void ws_setup_error_handler();

void ws_io_setup() {
	HAL_Init();

	ws_io_clock_setup();
	ws_io_i2c_setup();
	ws_io_dma_setup();
	ws_io_usart1_setup();
	ws_io_usart2_setup();
	ws_io_dma_setup();

	HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef) {
		.Pin = GPIO_PIN_5,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL
	});

	ws_backlog_alloc(WS_BACKLOG_SIZE);

#ifdef WS_DBG_PRINT_ESP_OVER_USART2
	ws_dbg_set_usart2_tty_color(WS_DBG_TTY_COLOR_DBGMSG);
	const char restart_str[] = "\r\n--- stm restart ---\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*) restart_str, strlen(restart_str), 100);
#endif

	ws_esp8266_ap_client_mode();
#ifdef WS_ESP8266_WLAN_MAC
	ws_esp8266_set_mac();
#endif
#ifdef WS_ESP8266_WLAN_IP
	ws_esp8266_set_ip();
#endif
	do ws_esp8266_connect(); while (g_ws_server_parser.last_response == WS_SERVER_RC_ERR);
	ws_esp8266_start_tcp_server();
}

static void ws_io_clock_setup() {
	if (HAL_OK != HAL_RCC_OscConfig(&(RCC_OscInitTypeDef){
		.OscillatorType = RCC_OSCILLATORTYPE_HSI,
		.HSIState = RCC_HSI_ON,
		.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
		.PLL.PLLState = RCC_PLL_ON,
		.PLL.PLLSource = RCC_PLLSOURCE_HSI,
		.PLL.PLLMUL = RCC_PLL_MUL12,
		.PLL.PREDIV = RCC_PREDIV_DIV2,
	})) return ws_setup_error_handler();

	if (HAL_OK != HAL_RCC_ClockConfig(&(RCC_ClkInitTypeDef){
		.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1,
		.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV1,
	}, FLASH_LATENCY_1)) return ws_setup_error_handler();

	if (HAL_RCCEx_PeriphCLKConfig(&(RCC_PeriphCLKInitTypeDef){
		.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1,
		.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1,
		.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI,
	}) != HAL_OK) return ws_setup_error_handler();
}

static void ws_io_i2c_setup() {
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
		return ws_setup_error_handler();
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
		return ws_setup_error_handler();
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
		return ws_setup_error_handler();
}

static void ws_io_usart1_setup() {
	if (HAL_UART_Init(&huart1) != HAL_OK)
		return ws_setup_error_handler();

	HAL_UART_Receive_DMA(&huart1, g_ws_esp8266_dma_rx_buffer, WS_DMA_RX_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // enable receive intterupts
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); // enable idle line detection
}

static void ws_io_usart2_setup() {
	if (HAL_UART_Init(&huart2) != HAL_OK)
		return ws_setup_error_handler();
} 

static void ws_io_dma_setup() {
	__HAL_RCC_DMA1_CLK_ENABLE();

	// interrupt priorities
	HAL_NVIC_SetPriority(DMA1_Ch1_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(DMA1_Ch1_IRQn);
	HAL_NVIC_SetPriority(DMA1_Ch2_3_DMA2_Ch1_2_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
}

void HAL_MspInit() {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_NVIC_SetPriority(PendSV_IRQn, 3, 0);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
  if(hi2c->Instance != I2C1) return;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_Init(GPIOB, &(GPIO_InitTypeDef) {
		.Pin = WS_PINOUT_I2C_SCL_PIN | WS_PINOUT_I2C_SDA_PIN,
		.Mode = GPIO_MODE_AF_OD,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_HIGH,
		.Alternate = GPIO_AF1_I2C1,
	});
	__HAL_RCC_I2C1_CLK_ENABLE();
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
	if(huart->Instance == USART1) {
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
    	.Pin = WS_PINOUT_USART1_TX_PIN | WS_PINOUT_USART1_RX_PIN,
    	.Mode = GPIO_MODE_AF_PP,
    	.Pull = GPIO_NOPULL,
    	.Speed = GPIO_SPEED_FREQ_HIGH,
    	.Alternate = GPIO_AF1_USART1,
		});

    // DMA setup
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) return ws_setup_error_handler();
    __HAL_DMA1_REMAP(HAL_DMA1_CH1_USART1_RX);
    __HAL_LINKDMA(huart, hdmarx, hdma_usart1_rx);
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK) return ws_setup_error_handler();
    __HAL_DMA1_REMAP(HAL_DMA1_CH2_USART1_TX);
    __HAL_LINKDMA(huart, hdmatx, hdma_usart1_tx);

    // USART1 interrupt Init
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
	} else if (huart->Instance == USART2) {
		__HAL_RCC_USART2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef) {
			.Pin = WS_PINOUT_USART2_TX_PIN | WS_PINOUT_USART2_RX_PIN,
			.Mode = GPIO_MODE_AF_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_LOW,
			.Alternate = GPIO_AF1_USART2,
		});
	}
}

void SysTick_Handler() {
	HAL_IncTick();
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) xPortSysTickHandler();
}

static void ws_setup_error_handler() {
	__disable_irq();
	for(;;);
}

void NMI_Handler() {
	for(;;);
}

void HardFault_Handler() {
	for(;;);
}
