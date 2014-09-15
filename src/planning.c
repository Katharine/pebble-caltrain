#include <pebble.h>
#include "planning.h"

void find_next_train(int count, TrainTime *times, TrainTime **nb, TrainTime **sb) {
  TrainTime *best[2] = {NULL, NULL};
  
  const time_t timestamp = time(NULL);
  const uint16_t minute = current_minute();
  const uint8_t day = current_day();
  
  for(int i = 0; i < count; ++i) {
    TrainTime *train_time = &times[i];
    TrainTrip trip;
    trip_get(train_time->trip, &trip);
    //APP_LOG(APP_LOG_LEVEL_INFO, "%d) %p time: %d, stop: %d, direction: %d", i, train_time, train_time->time, train_time->stop, trip.direction);
    
    // Make sure this trip is actually running today
    TrainCalendar *cal = calendar_get(trip.service);
    if((cal->days & day) == 0 || cal->start > timestamp || cal->end < timestamp) {
      continue;
    }
    
    // If it's better than the best we have so far, use it.
    if(train_time->time > minute) {
      if(best[trip.direction] == NULL || train_time->time < best[trip.direction]->time) {
        best[trip.direction] = train_time;
      }
    }
  }
  *nb = best[TrainDirectionNorthbound];
  *sb = best[TrainDirectionSouthbound];
}

uint16_t get_future_trains(uint8_t station, TrainDirection direction, TrainTime **trains) {
  uint16_t time_count = stop_times_count(station);
  uint16_t pointer = 0;
  
  TrainTime *times = malloc(time_count * sizeof(TrainTime));
  time_count = stop_get_times(station, time_count, times);
  
  *trains = malloc(time_count * sizeof(TrainTime));
  
  const time_t timestamp = time(NULL);
  const uint16_t minute = current_minute();
  const uint8_t day = current_day();
  
  for(int i = 0; i < time_count; ++i) {
    TrainTime *train_time = &times[i];
    TrainTrip trip;
    trip_get(train_time->trip, &trip);
    
    if(trip.direction != direction) {
      continue;
    }
    
    TrainCalendar *cal = calendar_get(trip.service);
    if((cal->days & day) == 0 || cal->start > timestamp || cal->end < timestamp) {
      continue;
    }
    
    if(train_time->time > minute) {
      memcpy(&((*trains)[pointer++]), train_time, sizeof(TrainTime));
    }
    
  }
  free(times);
  return pointer;
}

void next_train_at_station(uint8_t station, TrainTime *northbound, TrainTime *southbound) {
  uint16_t time_count = stop_times_count(station);
  TrainTime *times = malloc(time_count * sizeof(TrainTime));
  time_count = stop_get_times(station, time_count, times);
  
  TrainTime *north, *south;
  find_next_train(time_count, times, &north, &south);
  
  if(north != NULL) {
    memcpy(northbound, north, sizeof(TrainTime));
  } else {
    northbound->time = INVALID_TIME;
  }
  if(south != NULL) {
    memcpy(southbound, south, sizeof(TrainTime));
  } else {
    southbound->time = INVALID_TIME;
  }
  
  free(times);
}
