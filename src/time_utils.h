#pragma once

#include <pebble.h>
#include "model.h"

void train_time_format_minutes(uint16_t minutes, size_t n, char* buffer);
void train_time_format_state(TrainTime *train_time, size_t n, char* buffer);
