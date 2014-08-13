#pragma once
#include <pebble.h>
#include "model.h"

//! Given a set of times, finds the next one.
//! This is quite slow.
TrainTime* find_next_train(uint8_t direction, int count, TrainTime *times);

void next_train_at_station(uint8_t station, TrainTime *northbound, TrainTime *southbound);
