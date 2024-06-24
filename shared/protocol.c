#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "util.h"

#define WS_CMD_MAP(parsed_cmd, name, code) \
	if (strlen(parsed_cmd->argv[0]) == strlen(name) && strncmp(parsed_cmd->argv[0], name, strlen(name)) == 0) return code;

static ws_e_protocol_cmd ws_protocol_get_req_cmd_code(ws_s_protocol_parsed_req_cmd* parsed_cmd) {
	if (parsed_cmd == NULL) return WS_PROTOCOL_CMD_UNKNOWN; // invalid command
	WS_CMD_MAP(parsed_cmd, "last-records", WS_PROTOCOL_CMD_LAST_RECORDS);

	return WS_PROTOCOL_CMD_UNKNOWN;
}

void ws_protocol_parse_req_byte(ws_s_protocol_req_parser_state* state, char input) {
	switch(input) {
		case WS_PROTOCOL_C_EOL: {
			break;
		}

		case WS_PROTOCOL_C_SPACE: {
			if (!state->valid) return;
			state->arg_len++;
			return;
		}

		case WS_PROTOCOL_C_NULL: {
			state->valid = false;
			return;
		}

		default: {
			if (!state->valid) return;
			state->cmd[state->cmd_len++] = input;
			state->args_len[state->arg_len] += 1;
			if (state->cmd_len == WS_PROTOCOL_CMD_BUFFER_LEN) state->valid = false;
			return;
		}
	}
	// arg_len is used as an index while parsing, so add 1 to get length
	state->arg_len++;

	// parse cmd into argc and argv
	if (state->valid) ws_protocol_req_cmd_init(state);
	// create response
	ws_s_protocol_res* response = ws_protocol_parse_req_finished(state->target);

	// send response
	char response_first_line[16];
	sprintf(response_first_line, "%s,%x\n", response->success == WS_PROTOCOL_CMD_RETURN_OK ? "ok" : "error", response->msg->bytes);
	ws_protocol_send_data(response_first_line, strlen(response_first_line));
	if (!response->csh) ws_protocol_send_data((char*) response->msg->data, response->msg->bytes);
	else (*g_ws_protocol_res_handlers[response->cmd_code])(state->target, response, true);
	
	// free response data containers
	free(response->msg);
	free(response);

	// reset parser
	ws_protocol_req_parser_reset(state);

	return;
}

ws_s_protocol_res* ws_protocol_parse_req_finished(ws_s_protocol_parsed_req_cmd* parsed_cmd) {
	ws_s_protocol_res* response = malloc(sizeof(ws_s_protocol_res));
	response->success = WS_PROTOCOL_CMD_RETURN_ERROR;
	response->csh = false;
	response->msg = NULL;
	response->cmd_code = ws_protocol_get_req_cmd_code(parsed_cmd);

	if (response->cmd_code == WS_PROTOCOL_CMD_UNKNOWN) goto ws_protocol_parse_exit;
	if (response->cmd_code >= WS_PROTOCOL_CMD_AMOUNT) goto ws_protocol_parse_exit;

	ws_protocol_res_handler_t* ws_protocol_res_handler = g_ws_protocol_res_handlers[response->cmd_code];
	if (ws_protocol_res_handler == NULL) goto ws_protocol_parse_exit;
	(*ws_protocol_res_handler)(parsed_cmd, response, false);

ws_protocol_parse_exit:

	if (response->msg == NULL) response->msg = ws_bin_s_alloc(0);
	return response;
}

void ws_protocol_parse_req_bytes(ws_s_protocol_req_parser_state* state, char* input, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) ws_protocol_parse_req_byte(state, input[i]);
}

ws_s_protocol_req_parser_state* ws_protocol_req_parser_alloc() {
	ws_s_protocol_req_parser_state* parser_state = malloc(sizeof(ws_s_protocol_req_parser_state) + sizeof(uint16_t) * WS_PROTOCOL_CMD_MAX_ARGUMENTS);
	parser_state->cmd = malloc(sizeof(char) * WS_PROTOCOL_CMD_BUFFER_LEN);
	parser_state->target = NULL;
	ws_protocol_req_parser_reset(parser_state);
	return parser_state;
}

void ws_protocol_req_cmd_init(ws_s_protocol_req_parser_state* state) {
	state->target = malloc(sizeof(ws_s_protocol_parsed_req_cmd) + sizeof(char*) * state->arg_len);
	unsigned int args = WS_MIN(state->arg_len, WS_PROTOCOL_CMD_MAX_ARGUMENTS);
	for (unsigned int i = 0; i < args; i++)
		state->target->argv[i] = malloc(sizeof(char) * (state->args_len[i] + 1));

	state->target->argc = args;

	unsigned int head = 0;
	for (unsigned int i = 0; i < state->arg_len; i++) {
		strncpy(state->target->argv[i], &state->cmd[head], state->args_len[i]);
		state->target->argv[i][state->args_len[i]] = 0x00; // terminate argument with null byte
		head += state->args_len[i];
	}
}

void ws_protocol_req_parser_free(ws_s_protocol_req_parser_state* state) {
	if (state == NULL) return;
	if (state->target != NULL) ws_protocol_req_cmd_free(state->target);
	state->target = NULL;
	free(state->cmd);
	free(state);
	return;
}

void ws_protocol_req_parser_reset(ws_s_protocol_req_parser_state* state) {
	if (state->target != NULL) ws_protocol_req_cmd_free(state->target);
	state->target = NULL;
	state->valid = true;
	state->cmd_len = 0;
	state->arg_len = 0;
	memset(state->args_len, 0, sizeof(uint16_t) * WS_PROTOCOL_CMD_MAX_ARGUMENTS);
}

void ws_protocol_req_cmd_free(ws_s_protocol_parsed_req_cmd* cmd) {
	for (int i = 0; i < cmd->argc; i++)
		free(cmd->argv[i]);
	free(cmd);
	return;
}

unsigned short ws_protocol_get_header_size(ws_s_protocol_res* response) {
	unsigned short size = 2; // comma and trailing newline
	if (response->success == WS_PROTOCOL_CMD_RETURN_OK) size += 2; // ok
	if (response->success == WS_PROTOCOL_CMD_RETURN_ERROR) size += 5; // error
	size += ws_log16(response->msg->bytes) + 1; // amount of characters for message size (hex)
	return size;
}