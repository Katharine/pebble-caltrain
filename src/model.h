#pragma once

#define INVALID_TIME 65535
  
typedef enum {
  TrainDirectionSouthbound,
  TrainDirectionNorthbound,
} TrainDirection;
  
// uint8_t header
typedef struct {
  uint8_t zone;
  char name[18];
  int32_t lat;
  int32_t lon;
} __attribute__((packed)) TrainStop;

// uint16_t header
typedef struct {
  uint16_t trip;
  uint16_t time; // minutes since midnight on that day
  uint8_t stop;
  uint8_t sequence;
} __attribute__((packed)) TrainTime;

// uint16_t header
typedef struct {
  uint16_t trip_name;
  uint8_t direction;
  uint8_t route;
  uint8_t service;
} __attribute__((packed)) TrainTrip;

// uint8_t header
typedef struct {
  time_t start;
  time_t end;
  uint8_t days; // (1 << 0) = Monday; (1 << 6) = Sunday
} __attribute__((packed)) TrainCalendar;

//! Returns the number of stops.
uint8_t stop_count(void);

//! Produces a stop.
bool stop_get(uint8_t stop_id, TrainStop *stop);

uint16_t stop_times_count(uint8_t stop_id);
uint16_t stop_get_times(uint8_t stop_id, uint16_t time_count, TrainTime *train_times);

uint8_t trip_times_count(uint16_t trip_id);
uint8_t trip_get_times(uint16_t trip_id, uint8_t stop_count, TrainTime *trip_stops);

bool time_get(uint16_t time_id, TrainTime *time);

//! Produces a trip.
bool trip_get(uint16_t trip_id, TrainTrip *trip);

//! Produces a schedule entry.
//! The system owns the returned pointer; do not free it.
TrainCalendar* calendar_get(uint8_t calendar_id);

//! Get the current minute number.
uint16_t current_minute(void);

//! Get the current day mask.
uint8_t current_day(void);
