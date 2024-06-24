#pragma once

#include <stdint.h>

/**
 * @brief allocate backlog buffer and set global backlog pointer
 * @param record_amt amount of records to keep before overwriting oldest record
 */
void ws_backlog_alloc(uint16_t record_amt);

// enable struct packing
#pragma pack(push, 1)

/** @brief backlog record */
typedef struct {
	uint16_t id; /**< unique record identifier, numbered sequentially */
	uint8_t sens_temperature; /**< temperature reading */
	uint8_t sens_humidity; /**< humidity reading */
	uint8_t sens_atm_pressure; /**< atmospheric pressure reading */
} ws_s_backlog_record;

typedef struct {
	uint16_t buffer_size; /**< buffer size (amount of records) */
	uint16_t buffer_start; /** first record index */
	uint16_t buffer_end; /** last record index */
	ws_s_backlog_record records[]; /** record array */
} ws_s_backlog_database;

// disable struct packing
#pragma pack(pop)

/** @brief global record backlog database pointer */
extern ws_s_backlog_database* g_ws_backlog_database;

/**
 * @brief add record to database
 *
 * automatically sets record.id, pushes buffer_end forwards and overwrites the
 * last record if the buffer is full
 */
void ws_backlog_add_record(ws_s_backlog_record record);

/**
 * there's intentionally no function to retrieve multiple records as an array,
 * as this would either require
 * (a) copying the selection which is not possible with the current memory
 * constraints, or
 * (b) giving a direct pointer, but this would cause undefined behavior at the
 * ring buffer seam
 */

/** @brief get pointer to record with index `record_index` from the database */
ws_s_backlog_record* ws_backlog_get_record(uint16_t record_index);

/** @brief get pointer to last record with offset `record_offset` from the database */
ws_s_backlog_record* ws_backlog_get_last_record(uint16_t record_offset);

/** @brief return amount of valid records in database */
uint16_t ws_backlog_get_record_count();
