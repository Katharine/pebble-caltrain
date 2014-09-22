#pragma once
#include <pebble.h>
#include "model.h"

// Given a set of times, finds the next one.
// This is pretty slow and should be called sparingly.
void find_next_train(int count, TrainTime *times, TrainTime **nb, TrainTime **sb);

// Finds the next train in each direction at the given station.
// If there is no such train, the time will be set to INVALID_TIME.
void next_train_at_station(uint8_t station, TrainTime *northbound, TrainTime *southbound);

// Finds all future trains today for a given station in the given direction.
uint16_t get_future_trains(uint8_t station, TrainDirection direction, TrainTime **trains);
