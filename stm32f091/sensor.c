#include <FreeRTOS.h>
#include <task.h>
#include <stm32f0xx_hal.h>

#include "setup.h"
#include "sensor.h"
#include "backlog.h"

#define SI7021_ADDRESS ((uint16_t)(0x40) << 1)
#define SI7021_REG_TEMP ((uint8_t)(0xE3))
#define SI7021_REG_HUM ((uint8_t)(0xE5))
#define BMP280_ADDRESS ((uint16_t)(0x76) << 1)

uint8_t ws_sensor_temperature() {
	uint8_t buf[12];
	buf[0] = SI7021_REG_TEMP;
	HAL_I2C_Master_Transmit(&hi2c1, SI7021_ADDRESS, buf, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, SI7021_ADDRESS, buf, 2, HAL_MAX_DELAY);

	return ws_sensor_tmp_to_8(((int16_t)buf[0]<< 8 ) | (buf[1]));
}

uint8_t ws_sensor_humidity() {
	uint8_t buf[12];
	buf[0] = SI7021_REG_HUM;
	HAL_I2C_Master_Transmit(&hi2c1, SI7021_ADDRESS, buf, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, SI7021_ADDRESS, buf, 2, HAL_MAX_DELAY);

	return ws_sensor_hum_to_8(((int16_t)buf[0]<< 8 ) | (buf[1]));
}

uint8_t ws_sensor_atmospheric_pressure() {
	uint8_t buf[12];
	uint8_t buffer[12];

	buf[0]= 0xF4;
	buf[1]= 0x07;
	buffer[0] = 0xF7;
	HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDRESS, buf , 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDRESS, 0xF7, 1, buffer, 2, 100 );

	return ws_sensor_atm_to_8((uint16_t) buffer[0] << 8 | buffer[1]);
}

void ws_sensor_read() {
	ws_s_backlog_record record = {
		.sens_temperature = ws_sensor_temperature(),
		.sens_atm_pressure = ws_sensor_atmospheric_pressure(),
		.sens_humidity = ws_sensor_humidity()
	};
	ws_backlog_add_record(record);

	// < DEBUG PROTOCOL PARSING CODE >
	// ws_s_protocol_req_parser_state* parser = ws_protocol_req_parser_alloc();
	// const char* request = "last-records 5\n";
	// ws_protocol_parse_req_bytes(parser, (char*) request, strlen(request));
	// ws_protocol_req_parser_free(parser);
}

void ws_sensor_read_task() {
	while (1) {
		ws_sensor_read();
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		vTaskDelay(portTICK_PERIOD_MS * 1000 * 1);
	}
}

