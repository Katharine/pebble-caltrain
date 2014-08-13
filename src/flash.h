#pragma once

#include <pebble.h>
  
size_t flash_read_byte_range(ResHandle h, uint32_t start_offset, uint8_t *buffer, size_t num_bytes);
