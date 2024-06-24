#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief store kind of AT command response the server is expecting */
typedef enum {
	WS_SERVER_LM_CMD_ECHO, /** @brief listen for echo of sent command */
	WS_SERVER_LM_STATUS_CODE, /** @brief listen for busy, ERROR or OK */
	WS_SERVER_LM_IDLE, /** @brief listen for incoming +IPD commands */
	WS_SERVER_LM_IPD_LISTENING, /** @brief +IPD received, now reading data */
	WS_SERVER_LM_CIPSEND_LISTENING, /** @brief AT+CIPSEND sent, now reading data */
} ws_e_server_listen_mode;

/** @brief state machine for parsing +IPD command fields */
typedef enum {
	WS_SERVER_CL_CHANNEL_ID, /** @brief listen channel id */
	WS_SERVER_CL_DATA_LENGTH, /** @brief listen for data byte length */
	WS_SERVER_CL_DATA_READ, /** @brief listen for data and pipe to ws_protocol_parse_req_byte */
} ws_e_channel_listen_mode;

/** @brief AT command status codes */
typedef enum {
	WS_SERVER_RC_NONE = -1, /** @brief initial value */
	WS_SERVER_RC_BUSY, /** @brief status code busy p... */
	WS_SERVER_RC_ERR, /** @brief status code ERROR */
	WS_SERVER_RC_OK, /** @brief status code OK */
} ws_e_server_response_code;

/** @brief counters for byte-by-byte string comparison of AT response codes */
typedef struct {
	uint8_t s_ok; /** @brief counter for status code OK */
	uint8_t s_error; /** @brief counter for status code OK */
	uint8_t s_fail; /** @brief counter for status code OK */
	uint8_t s_busy; /** @brief counter for status code OK */
	uint8_t i_ipd; /** @brief counter for idle +IPD, */
	uint8_t i_prompt; /** @brief counter for idle > */
	uint8_t l_send_ok; /** @brief counter for ipd listen SEND OK */
	uint8_t l_error; /** @brief counter for ipd listen ERROR */
} ws_s_server_parser_response_counter;

/** @brief server parser statue struct */
typedef struct {
	ws_e_server_listen_mode mode; /** @brief expected AT response kind */
	ws_e_server_response_code last_response; /** @brief response code for last AT command */
	unsigned int current_channel; /** @brief TCP channel for last +IPD command */
	unsigned int channel_data_length; /** @brief data length for last +IPD command */
	unsigned int channel_data_counter; /** @brief amount of parsed data bytes for last +IPD command */
	ws_e_channel_listen_mode channel_listen_mode; /** @brief +IPD command parse state */
	bool channel_data_ignore; /** @brief whether last TCP request should be ignored because it's too long */
	ws_s_server_parser_response_counter rc; /** @brief response counters for byte-by-byte string comparison */
} ws_s_server_parser;

/** @brief global server parser struct */
extern ws_s_server_parser g_ws_server_parser;

/**
 * @brief +IPD incoming request handler
 *
 * this function takes chunks of data from the esp8266 and parses +IPD
 * commands. when a valid +IPD command is detected, it gets forwarded to
 * ws_protocol_parse_req_byte.
 *
 * @param data  pointer to data array
 * @param size  amount of bytes allowed to be read from `data`
 */
void ws_server_req_incoming(uint8_t* data, size_t size);

/** @brief send data to esp, waiting until server returns to idle mode */
void ws_server_send(uint8_t* data, size_t size);

/**
 * @brief parse byte from channel
 *
 * automatically creates parser struct and passes data onto protocol parser
 * functions
 *
 * @param channel  request channel
 * @param byte  data byte
 * @param ignore  ignore mode
 */
void ws_server_req_parse_byte(unsigned int channel, uint8_t byte, bool ignore);

/**
 * @brief close connection
 *
 * deallocates any parser struct that were automatically created in
 * ws_server_req_parse_byte
 *
 * @param channel  request channel
 * @param ignore  ignore mode
 */
void ws_server_req_finish(unsigned int channel, bool ignore);

/**
 * @brief append data to the tx buffer for sending response in chunks
 *
 * the esp8266 becomes unreliable when sending large tcp responses, so this
 * function buffers the complete response, and sends it in chunks broken up by
 * a newline ('\n') character
 *
 * to send data buffered using this function, refer to
 * `ws_server_buffer_request_chunk_send` and `ws_server_buffer_send_chunk`.
 *
 * @param data  pointer to data array
 * @param size  size of data array
 */
void ws_server_buffer_send_append(uint8_t* data, size_t size);

/** @brief send AT+CIPSEND with size of next chunk to the esp8266 */
void ws_server_buffer_request_chunk_send();
/** @brief send data chunk for last AT+CIPSEND command */
void ws_server_buffer_send_chunk();
