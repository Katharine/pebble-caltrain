#include <pebble.h>
#include "model.h"

static ResHandle s_stop_handle = NULL;
static uint8_t s_stop_count = 0;

static ResHandle get_stop_handle(void) {
  if(s_stop_handle == NULL) {
    s_stop_handle = resource_get_handle(RESOURCE_ID_DATA_STOPS);
  }
  return s_stop_handle;
}

uint8_t stop_count(void) {
  if(s_stop_count == 0) {
    resource_load_byte_range(get_stop_handle(), 0, &s_stop_count, 1);
  }
  return s_stop_count;
}

bool stop_get(uint8_t stop_id, TrainStop *stop) {
  if(stop_id >= stop_count()) {
    return false;
  }
  uint32_t offset = 1 + (stop_id * sizeof(TrainStop));
  return (resource_load_byte_range(get_stop_handle(), offset, (uint8_t *)stop, sizeof(TrainStop)) == sizeof(TrainStop));
}
