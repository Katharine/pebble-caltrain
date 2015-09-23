#include <pebble.h>
#include "flash.h"

// Take advantage of extra space on snowy.
#ifdef PBL_PLATFORM_APLITE
  #define BUFFER_SIZE 4096
#else
  #define BUFFER_SIZE 8192
#endif

static ResHandle s_handle = NULL;
static uint8_t s_buffer[BUFFER_SIZE];
static uint32_t s_offset = 0;
static uint32_t s_length = 0;

// Fills our in-memory cache of the resource starting at the given offset.
static void prv_reload_cache(ResHandle h, uint32_t start_offset) {
  s_handle = h;
  s_offset = start_offset;
  size_t res_size = resource_size(h);
  //APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Refilling cache for %p starting at %d.", h, (int)start_offset);
  if(res_size <= start_offset) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Request for illegal offset %d (of %d).", (int)s_offset, (int)res_size);
    s_length = 0;
    return;
  }
  size_t size = res_size - start_offset;
  if(size > sizeof(s_buffer)) {
    size = sizeof(s_buffer);
  }
  s_length = resource_load_byte_range(s_handle, s_offset, s_buffer, size);
}

size_t flash_read_byte_range(ResHandle h, uint32_t start_offset, uint8_t *buffer, size_t num_bytes) {
  // Don't bother for large chunks.
  if(num_bytes > sizeof(s_buffer)) {
    return resource_load_byte_range(h, start_offset, buffer, num_bytes);
  }
  // If we can't fill the request, refill our cache
  if(s_handle == NULL || s_handle != h || start_offset < s_offset || start_offset + num_bytes > s_offset + s_length) {
    prv_reload_cache(h, start_offset);
  }
  
  size_t bytes_available = s_length - (start_offset - s_offset);
  
  size_t n = num_bytes < bytes_available ? num_bytes : bytes_available;
  
  memcpy(buffer, s_buffer + (start_offset - s_offset), n);
  
  return n;
}
  