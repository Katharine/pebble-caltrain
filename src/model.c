#include <pebble.h>
#include "model.h"
#include "flash.h"
#include "colours.h"

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

static TrainCalendar *s_calendars = NULL;

static ResHandle prv_get_stop_handle(void) {
  static ResHandle s_handle = NULL;
  if(s_handle == NULL) {
    s_handle = resource_get_handle(RESOURCE_ID_DATA_STOPS);
  }
  return s_handle;
}

static ResHandle prv_get_stop_times_index_handle(void) {
  static ResHandle s_handle = NULL;
  if(s_handle == NULL) {
    s_handle = resource_get_handle(RESOURCE_ID_INDEX_STOP_TIMES);
  }
  return s_handle;
}

static ResHandle prv_get_time_handle(void) {
  static ResHandle s_handle = NULL;
  if(s_handle == NULL) {
    s_handle = resource_get_handle(RESOURCE_ID_DATA_TIMES);
  }
  return s_handle;
}

static ResHandle prv_get_trip_handle(void) {
  static ResHandle s_handle = NULL;
  if(s_handle == NULL) {
    s_handle = resource_get_handle(RESOURCE_ID_DATA_TRIPS);
  }
  return s_handle;
}

static ResHandle prv_get_trip_stops_index_handle(void) {
  static ResHandle s_handle = NULL;
  if(s_handle == NULL) {
    s_handle = resource_get_handle(RESOURCE_ID_INDEX_TRIP_STOPS);
  }
  return s_handle;
}

uint8_t stop_count(void) {
  static uint8_t s_stop_count = 0;
  if(s_stop_count == 0) {
    flash_read_byte_range(prv_get_stop_handle(), 0, &s_stop_count, 1);
  }
  return s_stop_count;
}

bool stop_get(uint8_t stop_id, TrainStop *stop) {
  if(stop_id >= stop_count()) {
    return false;
  }
  const uint32_t offset = 1 + (stop_id * sizeof(TrainStop));
  return (flash_read_byte_range(prv_get_stop_handle(), offset, (uint8_t *)stop, sizeof(TrainStop)) == sizeof(TrainStop));
}

uint16_t stop_times_count(uint8_t stop_id) {
  if(stop_id >= stop_count()) {
    return 0;
  }
  uint16_t count;
  flash_read_byte_range(prv_get_stop_times_index_handle(), 1 + 4*stop_id + 2, (uint8_t *)&count, sizeof(count));
  return count;
}

uint16_t stop_get_times(uint8_t stop_id, uint16_t time_count, TrainTime *train_times) {
  if(stop_id >= stop_count()) {
    return 0;
  }
  time_t start = time(NULL);
  ResHandle h = prv_get_stop_times_index_handle();
  
  // First get the number of entries and the offset into the index file
  uint16_t stop_data[2];
  flash_read_byte_range(h, 1 + 4*stop_id, (uint8_t *)&stop_data, 4);
  const uint16_t count = MIN(time_count, stop_data[1]);
  const uint16_t index_read_offset = stop_data[0];
  const uint32_t index_read_size = count * 2;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Stop time index has %d bytes at offset %d.", (int)index_read_size, (int)index_read_offset);
  
  // Read the index
  uint16_t *time_indexes = malloc(index_read_size);
  flash_read_byte_range(h, index_read_offset, (uint8_t *)time_indexes, index_read_size);
  
  // Fetch each time listed.
  for(int i = 0; i < count; ++i) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d) Fetching time #%d", i, time_indexes[i]);
    time_get(time_indexes[i], &train_times[i]);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "#%d: trip: %d, time: %d, stop: %d, sequence: %d", time_indexes[i], train_times[i].trip, train_times[i].time, train_times[i].stop, train_times[i].sequence);
  }
  
  free(time_indexes);
  time_t end = time(NULL);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "stop_get_times took %d seconds.", (int)(end - start));
  
  return count;
}

uint8_t trip_times_count(uint16_t trip_id) {
  uint8_t count;
  flash_read_byte_range(prv_get_trip_stops_index_handle(), 2 + 3*trip_id + 2, &count, sizeof(count));
  return count;
}

uint8_t trip_get_times(uint16_t trip_id, uint8_t stop_count, TrainTime *trip_times) {
  uint8_t trip_data[3];
  resource_load_byte_range(prv_get_trip_stops_index_handle(), 2 + 3*trip_id, trip_data, 3);
  const uint16_t offset = (trip_data[1] << 8) | trip_data[0];
  const uint8_t count = MIN(stop_count, trip_data[2]);
  
  uint16_t *time_indexes = malloc(count*2);
  resource_load_byte_range(prv_get_trip_stops_index_handle(), offset, (uint8_t *)time_indexes, count*2);
  
  for(int i = 0; i < count; ++i) {
    time_get(time_indexes[i], &trip_times[i]);
  }
  
  free(time_indexes);
  
  return count;
}

bool time_get(uint16_t time_id, TrainTime *time) {
  return (flash_read_byte_range(prv_get_time_handle(), 2 + time_id*sizeof(TrainTime), (uint8_t *)time, sizeof(TrainTime)) == sizeof(TrainTime));
}

bool trip_get(uint16_t trip_id, TrainTrip *trip) {
  // Instead of reading this every time, just read the whole thing in once and copy from RAM.
  static TrainTrip *s_trips = NULL;
  if(s_trips == NULL) {
    uint16_t size;
    flash_read_byte_range(prv_get_trip_handle(), 0, (uint8_t *)&size, 2);
    s_trips = malloc(size * sizeof(TrainTrip));
    flash_read_byte_range(prv_get_trip_handle(), 2, (uint8_t *)s_trips, size * sizeof(TrainTrip));
  }
  memcpy(trip, &s_trips[trip_id], sizeof(TrainTrip));
  return true;
}

GColor trip_get_colour(TrainTrip *trip) {
  #ifdef PBL_COLOR
    switch(trip->route) {
      case 0: return COLOUR_BULLET;
      case 1: return COLOUR_LIMITED;
      case 2: return COLOUR_LOCAL;
      case 3: return COLOUR_SHUTTLE;
      default: return COLOUR_SHUTTLE;
    }
  #else
    return GColorWhite;
  #endif
}

static void prv_load_calendars(void) {
  static uint8_t s_calendar_count = 0;

  if(s_calendars != NULL) {
    return;
  }
  const ResHandle h = resource_get_handle(RESOURCE_ID_DATA_CALENDAR);
  resource_load_byte_range(h, 0, &s_calendar_count, 1);
  const uint16_t calendar_size = s_calendar_count * sizeof(TrainCalendar);
  s_calendars = malloc(calendar_size);
  resource_load_byte_range(h, 1, (uint8_t *)s_calendars, calendar_size);
}

TrainCalendar* calendar_get(uint8_t calendar_id) {
  prv_load_calendars();
  return &s_calendars[calendar_id];
}

uint16_t current_minute(void) {
  const time_t timestamp = time(NULL);
  struct tm* t = localtime(&timestamp);
  // Until 2am, consider it to be sometime after 24:00 yesterday.
  if(t->tm_hour < 2) {
    t->tm_hour += 24; // This may be a violation of the spec?
  }
  return (t->tm_hour * 60 + t->tm_min);
}

uint8_t current_day(void) {
  const time_t timestamp = time(NULL);
  struct tm* t = localtime(&timestamp);
  // Until 2am, consider it to be sometime after 24:00 yesterday.
  if(t->tm_hour < 2) {
    t->tm_wday = ((t->tm_wday + 6) % 7); // i.e. day -= 1
  }
  return 1 << ((t->tm_wday + 6) % 7);
}
