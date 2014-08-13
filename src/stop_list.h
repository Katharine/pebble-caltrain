#pragma once
#include <pebble.h>

void show_stop_list(void);
void hide_stop_list(void);
void select_list_select_stop(uint8_t stop_id, bool show_window);
void show_nearest_stop(int32_t longitude, int32_t latitude);
