#pragma once

#include <stdint.h>

/** @brief binary data container with length */
typedef struct {
  uint16_t bytes;
  uint8_t data[];
} ws_s_bin;

/** @brief allocate new ws_s_bin struct */
ws_s_bin *ws_bin_s_alloc(uint16_t bytes);

