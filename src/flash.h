#pragma once

#include <pebble.h>

// Reads num_bytes bytes from resource h at start_offset into buffer.
// This behaves exactly the same as resource_load_byte_range, but for performance reasons
// it always reads a large block, then attempts to fill future requests from that memory if possible.
size_t flash_read_byte_range(ResHandle h, uint32_t start_offset, uint8_t *buffer, size_t num_bytes);
