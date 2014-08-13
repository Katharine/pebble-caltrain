#pragma once
#include <pebble.h>
#include "model.h"

//! Given a set of times, finds the next one.
//! This is quite slow.
void find_next_train(int count, TrainTime *times, TrainTime **nb, TrainTime **sb);

void next_train_at_station(uint8_t station, TrainTime *northbound, TrainTime *southbound);
