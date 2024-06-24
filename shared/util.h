#pragma once

#include <stdint.h>

#define WS_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define WS_MAX(a, b) (((a) > (b)) ? (a) : (b))

/** @brief take the log base 16 of `x` */
unsigned int ws_log16(unsigned int x);

#ifdef __cplusplus
extern "C" {
#endif

/** @brief convert 16-bit temperature value to uint8_t */
uint8_t ws_sensor_tmp_to_8(uint16_t temperature);
/** @brief convert 16-bit humidity value to uint8_t */
uint8_t ws_sensor_hum_to_8(uint16_t humidity);
/** @brief convert 16-bit atmospheric pressure value to uint8_t */
uint8_t ws_sensor_atm_to_8(uint16_t atmospheric_pressure);

/** @brief convert 8-bit temperature value back to float */
float ws_sensor_tmp_to_f(uint8_t temperature);
/** @brief convert 8-bit humidity value back to float */
float ws_sensor_hum_to_f(uint8_t humidity);
/** @brief convert 8-bit atmospheric pressure value back to float */
float ws_sensor_atm_to_f(uint8_t atmospheric_pressure);

#ifdef __cplusplus
}
#endif

