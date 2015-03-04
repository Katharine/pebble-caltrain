#pragma once
#include <pebble.h>

void menu_hack_set_colours(GContext *ctx, MenuLayer *menu, MenuIndex *index);

// This function WILL break for anything other than 2.9.0 or 3.0-dp1.
void menu_hack_disable_inversion(MenuLayer *menu);

int16_t menu_hack_borderless_cells(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
