#include "util.h"

unsigned int ws_log16(unsigned int x) {
    unsigned int l = 0;
    while (x >>= 4) ++l; // bitshift right by 4 until x == 0
    return l;
}

#ifdef __cplusplus
extern "C" {
#endif

uint8_t ws_sensor_tmp_to_8(uint16_t temperature) {
	return temperature / 256;
}

uint8_t ws_sensor_hum_to_8(uint16_t humidity) {
	return humidity / 256;
}

uint8_t ws_sensor_atm_to_8(uint16_t atmospheric_pressure) {
	return atmospheric_pressure / 256;
}

float ws_sensor_tmp_to_f(uint8_t temperature) {
	return ((175.72 * temperature) / 256.0) - 46.85;
}

float ws_sensor_hum_to_f(uint8_t humidity) {
	return ((125.0 * humidity) / 256.0) - 6.0;
}

float ws_sensor_atm_to_f(uint8_t atmospheric_pressure) {
	return ((20.0 * atmospheric_pressure) / 256.0) + 990.0; // datasheet no?
}

#ifdef __cplusplus
}
#endif

