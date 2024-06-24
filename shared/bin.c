#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

#include "bin.h"

ws_s_bin *ws_bin_s_alloc(uint16_t bytes) {
	ws_s_bin *temp = malloc(sizeof(ws_s_bin) + sizeof(uint8_t) * bytes);
	temp->bytes	   = bytes;
	return temp;
}
