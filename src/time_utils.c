#include <pebble.h>
#include "time_utils.h"

void train_time_format_minutes(uint16_t minutes, size_t n, char* buffer) {
  minutes = minutes % (60 * 24);
  snprintf(buffer, n, "%02d:%02d", (int)(minutes / 60), (int)(minutes % 60));
}

void train_time_format_state(TrainTime *train_time, size_t n, char* buffer) {
  uint16_t now = current_minute();
  int16_t diff = train_time->time - now;
  if(diff > 0) {
    snprintf(buffer, n, "%d min", diff);
  } else if(diff == 0) {
    strncpy(buffer, "Now", n);
  } else {
    strncpy(buffer, "", n);
  }
}
