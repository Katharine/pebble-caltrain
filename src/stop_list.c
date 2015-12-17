#include "stop_list.h"
#include <pebble.h>

#include "model.h"
#include "stop_info.h"
#include "persist.h"
#include "colours.h"
#include "menu_hacks.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_menu;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, PBL_IF_ROUND_ELSE(true, false));
  window_set_background_color(s_window, COLOUR_WINDOW);
  
  // s_menu
  GRect menu_bounds = layer_get_bounds(window_get_root_layer(s_window));
#ifdef PBL_ROUND
  menu_bounds = grect_inset(menu_bounds, GEdgeInsets(STATUS_BAR_LAYER_HEIGHT, 0));
#endif
  s_menu = menu_layer_create(menu_bounds);
  if(watch_info_get_firmware_version().major >= 3) {
    scroll_layer_set_shadow_hidden(menu_layer_get_scroll_layer(s_menu), true);
  }
  menu_set_colours(s_menu);
#ifdef PBL_ROUND
  menu_layer_set_center_focused(s_menu, true);
#endif
  menu_layer_set_click_config_onto_window(s_menu, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menu);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_menu);
}
// END AUTO-GENERATED UI CODE

static void prv_handle_window_unload(Window* window) {
  persist_selected_stop(menu_layer_get_selected_index(s_menu).row);
  destroy_ui();
}

static void prv_draw_menu_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *menu) {
  TrainStop stop;
  stop_get(cell_index->row, &stop);
  char zone[] = "zone 50";
  snprintf(zone, sizeof(zone), "Zone %d", (int)stop.zone);

  menu_cell_basic_draw(ctx, cell_layer, stop.name, zone, NULL);
}

static uint16_t prv_get_menu_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return stop_count();
}

static void prv_handle_menu_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  show_stop_info(cell_index->row);
}

void select_list_select_stop(uint8_t stop_id, bool show_window) {
  menu_layer_set_selected_index(s_menu, (MenuIndex){.section=0, .row=stop_id}, MenuRowAlignCenter, false);
  if(show_window) {
    show_stop_info(stop_id);
  }
}

static uint64_t prv_dist_sq(int32_t lon1, int32_t lat1, int32_t lon2, int32_t lat2) {
  int64_t lon_diff = lon1 - lon2;
  int64_t lat_diff = lat1 - lat2;
  return lon_diff * lon_diff + lat_diff * lat_diff;
}

void show_nearest_stop(int32_t lon, int32_t lat) {
  // If the user picked something else while we were thinking, don't override.
  if(window_stack_get_top_window() != s_window) {
    return;
  }
  APP_LOG(APP_LOG_LEVEL_INFO, "Finding closest stop to %d, %d", (int)lon, (int)lat);
  uint8_t count = stop_count();
  // Linear search to find the closest station, because that's good enough at this scale.
  uint64_t best_dist = UINT64_MAX; // that's 2^64-1, if you don't have that memorised.
  uint8_t best_id = UINT8_MAX;
  for(uint8_t i = 0; i < count; ++i) {
    TrainStop stop;
    stop_get(i, &stop);
    uint64_t dist = prv_dist_sq(lon, lat, stop.lon, stop.lat);
    if(dist < best_dist) {
      best_dist = dist;
      best_id = i;
    }
  }
  if(best_id == UINT8_MAX) {
    return;
  }
  select_list_select_stop(best_id, true);
}

int16_t prv_menu_round_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index,
                             void *callback_context) {
  if (menu_layer_get_selected_index(menu_layer).row == cell_index->row) {
    return MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT;
  } else {
    return MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT;
  }
}

void show_stop_list(void) {
  initialise_ui();
  
  menu_layer_set_callbacks(s_menu, s_menu, (MenuLayerCallbacks){
    .draw_row = prv_draw_menu_row,
    .get_num_rows = prv_get_menu_rows,
    .select_click = prv_handle_menu_click,
#ifdef PBL_ROUND
    .get_cell_height = prv_menu_round_cell_height,
#endif
  });
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = prv_handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_stop_list(void) {
  window_stack_remove(s_window, true);
}
