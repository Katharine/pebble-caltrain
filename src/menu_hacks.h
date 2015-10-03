#pragma once
#include <pebble.h>

void menu_set_colours(MenuLayer *menu);
int16_t menu_hack_borderless_cells(struct MenuLayer *menu_layer, MenuIndex *cell_index,
                                   void *callback_context);
