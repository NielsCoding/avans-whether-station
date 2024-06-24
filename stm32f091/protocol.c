#include <stdio.h>

#include "../shared/protocol.h"
#include "backlog.h"
#include "util.h"
#include "server.h"
#include "esp8266.h"

void ws_protocol_res_last_records(ws_s_protocol_parsed_req_cmd* parsed_cmd, ws_s_protocol_res* response, bool send) {
	static unsigned int record_amount = 0;
	static unsigned int record_offset = 0;
	const char* response_header = "id,temperature,humidity,atmospheric_pressure\n";
	const unsigned int response_line_len = strlen("xxxx,xx,xx,xx\n");

	if (!send) {
		response->success = WS_PROTOCOL_CMD_RETURN_OK;
		response->csh = true;
		response->msg = ws_bin_s_alloc(0);
		response->msg->bytes = 0;
		if (sscanf(parsed_cmd->argv[1], "%x", &record_amount) < 1) response->success = WS_PROTOCOL_CMD_RETURN_ERROR;
		if (sscanf(parsed_cmd->argv[2], "%x", &record_offset) < 1) response->success = WS_PROTOCOL_CMD_RETURN_ERROR;
		else {
			record_amount = WS_MIN(record_amount + record_offset, ws_backlog_get_record_count());
			record_amount = WS_MAX(0, (int) record_amount - (int) record_offset);
			response->msg->bytes = strlen(response_header) + response_line_len * record_amount;
		}
	} else {
		if (response->success == WS_PROTOCOL_CMD_RETURN_ERROR) return;
		ws_protocol_send_data(response_header, strlen(response_header));
		char line[response_line_len + 1]; // + 1 for string terminator
		for (unsigned int i = 0; i < record_amount; i++) {
			ws_s_backlog_record* record = ws_backlog_get_last_record(i + record_offset);
			sprintf(line, "%04x,%02x,%02x,%02x\n", record->id, record->sens_temperature, record->sens_humidity, record->sens_atm_pressure);
			ws_protocol_send_data(line, response_line_len);
		}
	}
}

void ws_protocol_send_data(const char* data, unsigned int length) {
	ws_server_buffer_send_append((uint8_t*) data, length);
}
