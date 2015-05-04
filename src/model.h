#pragma once
#include <pebble.h>
#include "sdk_hacks.h"

#define INVALID_TIME UINT16_MAX

// Indicates which direction a train is going in.
typedef enum {
  TrainDirectionSouthbound,
  TrainDirectionNorthbound,
} TrainDirection;

// Describes a physical station.
// uint8_t header
typedef struct {
  uint8_t zone;  // Zone number
  char name[18]; // Station name
  int32_t lat;   // Station latitude
  int32_t lon;   // Station longitude
} __attribute__((packed)) TrainStop;

// Describes a time at which a given train departs a given stop.
// uint16_t header
typedef struct {
  uint16_t trip;    // TrainTrip ID
  uint16_t time;    // minutes since midnight on that day
  uint8_t stop;     // TrainStop ID
  uint8_t sequence; // The order of this time on the trip, 1-based.
} __attribute__((packed)) TrainTime;

// Describes a single trip of a train.
// uint16_t header
typedef struct {
  uint16_t trip_name; // The trip's name, generally a three-digit number.
  uint8_t direction;  // Northbound or southbound
  uint8_t route;      // A TrainCalendar reference indicating when this trip runs.
  uint8_t service;    // The service type (e.g. bullet, limited, local, shuttle)
} __attribute__((packed)) TrainTrip;

// uint8_t header
typedef struct {
  time_t start; // Unix timestamp indicating when this calendar becomes valid.
  time_t end;   // Unix timestamp indicating when this calendar becomes invalid
  uint8_t days; // Bitmap indicating days this calendar occurs. (1 << 0) = Monday; (1 << 6) = Sunday
} __attribute__((packed)) TrainCalendar;

// Returns the number of stops.
uint8_t stop_count(void);

// Reads TrainStop stop_id into stop, returning true if successful.
bool stop_get(uint8_t stop_id, TrainStop *stop);

// Returns the number of TrainTimes that exist for stop_id
uint16_t stop_times_count(uint8_t stop_id);
// Places up to time_count TrainTimes for stop_id in train_times, and returns the
// number of times.
uint16_t stop_get_times(uint8_t stop_id, uint16_t time_count, TrainTime *train_times);

// Returns the number of TrainTimes that exist for trip_id
uint8_t trip_times_count(uint16_t trip_id);
// Places up to time_count TrainTimes for trip_id in trip_stops, and returns the
// number of times.
uint8_t trip_get_times(uint16_t trip_id, uint8_t time_count, TrainTime *trip_stops);

// Returns the expected colour of a trip.
GColor trip_get_colour(TrainTrip *trip);

// Reads TrainTime time_id into time, returning true if successful.
bool time_get(uint16_t time_id, TrainTime *time);

// Reads TrainTrip trip_id into trip, returning true if successful.
bool trip_get(uint16_t trip_id, TrainTrip *trip);

// Returns a schedule entry.
// The system owns the returned pointer; do not free it.
TrainCalendar* calendar_get(uint8_t calendar_id);

// Returns the current minute number.
uint16_t current_minute(void);

// Returns the current day mask.
uint8_t current_day(void);
