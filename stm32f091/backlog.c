#include <stdlib.h>

#include "backlog.h"

ws_s_backlog_database* g_ws_backlog_database = NULL;

void ws_backlog_alloc(uint16_t record_amt) {
	g_ws_backlog_database = malloc(sizeof(ws_s_backlog_database) + sizeof(ws_s_backlog_record) * record_amt);
	g_ws_backlog_database->buffer_size = record_amt;
	g_ws_backlog_database->buffer_start = 0;
	g_ws_backlog_database->buffer_end = 0;
}

void ws_backlog_add_record(ws_s_backlog_record record) {
	static uint16_t id = 0;

	g_ws_backlog_database->records[g_ws_backlog_database->buffer_end].id = id++;
	g_ws_backlog_database->records[g_ws_backlog_database->buffer_end].sens_atm_pressure = record.sens_atm_pressure;
	g_ws_backlog_database->records[g_ws_backlog_database->buffer_end].sens_humidity = record.sens_humidity;
	g_ws_backlog_database->records[g_ws_backlog_database->buffer_end].sens_temperature = record.sens_temperature;

	// shift buffer start/end
	g_ws_backlog_database->buffer_end = (g_ws_backlog_database->buffer_end + 1) % g_ws_backlog_database->buffer_size;
	if (g_ws_backlog_database->buffer_end == g_ws_backlog_database->buffer_start)
		g_ws_backlog_database->buffer_start = (g_ws_backlog_database->buffer_start + 1) % g_ws_backlog_database->buffer_size;
}

ws_s_backlog_record* ws_backlog_get_record(uint16_t record_index) {
	return &g_ws_backlog_database->records[record_index];
}

ws_s_backlog_record* ws_backlog_get_last_record(uint16_t record_offset) {
	return ws_backlog_get_record((g_ws_backlog_database->buffer_end - record_offset - 1) % g_ws_backlog_database->buffer_size);
}

uint16_t ws_backlog_get_record_count() {
	// add buffer_size to the result of the modulo operation if it's result is negative
	// (only works when buffer_size is less than 2^15)
	// this is a consequence of the way in which c handles negative numbers in modulo operations
	int16_t mod = (g_ws_backlog_database->buffer_end - g_ws_backlog_database->buffer_start) % g_ws_backlog_database->buffer_size;
	return mod < 0 ? mod + g_ws_backlog_database->buffer_size : mod;
}
