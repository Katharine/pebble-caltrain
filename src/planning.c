#include <pebble.h>
#include "planning.h"

TrainTime* find_next_train(uint8_t direction, int count, TrainTime *times) {
  TrainTime *best = NULL;
  const time_t timestamp = time(NULL);
  struct tm* t = localtime(&timestamp);
  // Until 2am, consider it to be sometime after 24:00 yesterday.
  if(t->tm_hour < 2) {
    t->tm_hour += 24; // This may be a violation of the spec?
    t->tm_wday = ((t->tm_wday + 6) % 7); // i.e. day -= 1
  }
  const uint16_t current_minute = t->tm_hour * 60 + t->tm_min;
  const uint8_t day = 1 << ((t->tm_wday + 6) % 7);
  APP_LOG(APP_LOG_LEVEL_INFO, "Current day: %d; minute: %d", day, current_minute);
  time_t start = time(NULL);
  for(int i = 0; i < count; ++i) {
    TrainTime *train_time = &times[i];
    TrainTrip trip;
    trip_get(train_time->trip, &trip);
    //APP_LOG(APP_LOG_LEVEL_INFO, "%d) %p time: %d, stop: %d, direction: %d", i, train_time, train_time->time, train_time->stop, trip.direction);
    
    // Make sure we're going in the right direction.
    if(trip.direction != direction) {
      continue;
    }
    
    // Make sure this trip is actually running today
    TrainCalendar *cal = calendar_get(trip.service);
    if((cal->days & day) == 0 || cal->start > timestamp || cal->end < timestamp) {
      continue;
    }
    
    // If it's better than the best we have so far, use it.
    if(train_time->time > current_minute) {
      if(best == NULL || train_time->time < best->time) {
        best = train_time;
      }
    }
  }
  int time_taken = time(NULL) - start;
  APP_LOG(APP_LOG_LEVEL_INFO, "Scanning took %d seconds.", time_taken);
  return best;
}

void next_train_at_station(uint8_t station, TrainTime *northbound, TrainTime *southbound) {
  uint16_t time_count = stop_times_count(station);
  APP_LOG(APP_LOG_LEVEL_INFO, "Station has %d times.", time_count);
  TrainTime *times = malloc(time_count * sizeof(TrainTime));
  time_count = stop_get_times(station, time_count, times);
  
  TrainTime *north = find_next_train(TrainDirectionNorthbound, time_count, times);
  TrainTime *south = find_next_train(TrainDirectionSouthbound, time_count, times);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "North: %p, South: %p", north, south);
  
  memcpy(northbound, north, sizeof(TrainTime));
  memcpy(southbound, south, sizeof(TrainTime));
  
  APP_LOG(APP_LOG_LEVEL_INFO, "trip North: %d; south: %d", northbound->trip, southbound->trip);
  APP_LOG(APP_LOG_LEVEL_INFO, "time North: %d; south: %d", northbound->time, southbound->time);
  
  free(times);
}
