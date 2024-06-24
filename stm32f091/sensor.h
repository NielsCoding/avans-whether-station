#pragma once

#include <stdint.h>

/** Get current temperature measurement */
uint8_t ws_sensor_temperature();
/** Get current humidity measurement */
uint8_t ws_sensor_humidity();
/** Get current atmospheric pressure measurement */
uint8_t ws_sensor_atmospheric_pressure();

/**
 * Put measurements from every sensor into a `ws_s_backlog_record`, and commit
 * that struct to the database
 */
void ws_sensor_read();

/** FreeRTOS task that runs `ws_sensor_read` every minute */
void ws_sensor_read_task();

