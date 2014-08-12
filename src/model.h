#pragma once

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
