#include "stop_info.h"
#include <pebble.h>

#include "model.h"
#include "planning.h"
#include "persist.h"
#include "next_train_layer.h"
#include "train_times_list.h"
#include "colours.h"

static uint8_t s_stop_id;
static TrainStop s_stop;
static TrainTime s_northbound;
static TrainTime s_southbound;

static NextTrainLayer *s_nb_layer;
static NextTrainLayer *s_sb_layer;
#ifdef PBL_ROUND
static StatusBarLayer *s_status_bar;
static ActionMenu *s_action_menu;
#endif

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_24_bold;
static TextLayer *s_station_name;
static InverterLayer *s_inverterlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, PBL_IF_RECT_ELSE(false, true));
  window_set_background_color(s_window, COLOUR_WINDOW);
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  // s_station_name
  s_station_name = text_layer_create(PBL_IF_RECT_ELSE(GRect(0, -8, 144, 28), GRect(0, 18, 180, 28)));
  text_layer_set_background_color(s_station_name, GColorClear);
  text_layer_set_text_color(s_station_name, COLOUR_HEADER_TEXT);
  text_layer_set_text(s_station_name, "Mountain View");
  text_layer_set_text_alignment(s_station_name, GTextAlignmentCenter);
  text_layer_set_font(s_station_name, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_station_name);
  
  // s_inverterlayer_1

  s_inverterlayer_1 = inverter_layer_create(PBL_IF_RECT_ELSE(GRect(0, 86, 144, 2), GRect(89, 74, 2, 92)));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_station_name);
  inverter_layer_destroy(s_inverterlayer_1);
  s_window = NULL;
}
// END AUTO-GENERATED UI CODE

static void prv_draw_title_backing(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, COLOUR_HEADER);
  #ifdef PBL_ROUND
    graphics_fill_circle(ctx, GPoint(90, -26), 90);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, GPoint(188, 90), 15);
    graphics_fill_rect(ctx, GRect(88, 77, 2, 85), 0, GCornerNone);
  #else
    graphics_fill_rect(ctx, GRect(0, 0, 144, 20), 0, GCornerNone);
  #endif
}

static void prv_init_custom_ui(void) {
#ifndef PBL_ROUND
  s_nb_layer = next_train_layer_create(GRect(0, 14, 144, 71), "NB", NextTrainLayerStyleHorizontal);
  s_sb_layer = next_train_layer_create(GRect(0, 80, 144, 71), "SB", NextTrainLayerStyleHorizontal);
#else
  s_nb_layer = next_train_layer_create(GRect(10, 60, 74, 112), "NB", NextTrainLayerStyleRightAligned);
  s_sb_layer = next_train_layer_create(GRect(96, 60, 74, 112), "SB", NextTrainLayerStyleLeftAligned);
  s_status_bar = status_bar_layer_create();
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_status_bar);
  status_bar_layer_set_colors(s_status_bar, GColorClear, GColorWhite);
#endif
  
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_nb_layer);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sb_layer);
  layer_set_update_proc(window_get_root_layer(s_window), prv_draw_title_backing);
}

static void prv_destroy_custom_ui(void) {
  next_train_layer_destroy(s_nb_layer);
  next_train_layer_destroy(s_sb_layer);
}

static void prv_handle_window_unload(Window* window) {
  tick_timer_service_unsubscribe();
  prv_destroy_custom_ui();
  destroy_ui();
  persist_hiding_stop();
}

static void prv_update_ui(void) {
  next_train_layer_set_time(s_nb_layer, &s_northbound);
  next_train_layer_set_time(s_sb_layer, &s_southbound);
}

static void prv_update_times(void) {
  next_train_at_station(s_stop_id, &s_northbound, &s_southbound);
  prv_update_ui();
}

static void prv_handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  uint16_t now = current_minute();
  if(now > s_northbound.time || now > s_southbound.time) {
    prv_update_times();
  } else {
    prv_update_ui();
  }
}

static void prv_handle_up_click(ClickRecognizerRef recognizer, void *context) {
  show_train_times_list(s_stop_id, TrainDirectionNorthbound);
}

static void prv_handle_down_click(ClickRecognizerRef recognizer, void *context) {
  show_train_times_list(s_stop_id, TrainDirectionSouthbound);
}

#ifdef PBL_ROUND
static void prv_handle_action(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  TrainDirection direction = (TrainDirection)action_menu_item_get_action_data(action);
  show_train_times_list(s_stop_id, direction);
}

static void prv_menu_did_close(ActionMenu *menu, const ActionMenuItem *performed_action, void *context) {
  action_menu_hierarchy_destroy(action_menu_get_root_level(menu), NULL, NULL);
}

static void prv_show_action_menu(void) {
  ActionMenuLevel *root = action_menu_level_create(2);
  action_menu_level_add_action(root, "Northbound", prv_handle_action, (void *)TrainDirectionNorthbound);
  action_menu_level_add_action(root, "Southbound", prv_handle_action, (void *)TrainDirectionSouthbound);
  s_action_menu = action_menu_open(&(ActionMenuConfig){
    .root_level = root,
    .colors = {
      .background = GColorBlack,
      .foreground = GColorRed,
    },
    .did_close = prv_menu_did_close,
    .align = ActionMenuAlignCenter,
  });
}

static void prv_handle_select_click(ClickRecognizerRef recognizer, void *context) {
  prv_show_action_menu();
}
#endif

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, prv_handle_up_click);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_handle_down_click);
  #ifdef PBL_ROUND
    window_single_click_subscribe(BUTTON_ID_SELECT, prv_handle_select_click);
  #endif
}

void show_stop_info(uint8_t stop_id) {
  if(!stop_get(stop_id, &s_stop)) {
    return;
  }
  hide_stop_info(); // this avoids a potential race condition.
  s_stop_id = stop_id;
  initialise_ui();
  prv_init_custom_ui();
  text_layer_set_text(s_station_name, s_stop.name);
  prv_update_times();
  tick_timer_service_subscribe(MINUTE_UNIT, prv_handle_tick);
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = prv_handle_window_unload,
  });
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_stack_push(s_window, true);
  persist_showing_stop(stop_id);
}

void hide_stop_info(void) {
  window_stack_remove(s_window, true);
}
