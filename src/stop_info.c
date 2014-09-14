#include "stop_info.h"
#include <pebble.h>

#include "model.h"
#include "planning.h"
#include "persist.h"
#include "next_train_layer.h"

static uint8_t s_stop_id;
static TrainStop s_stop;
static TrainTime s_northbound;
static TrainTime s_southbound;

static NextTrainLayer *s_nb_layer;
static NextTrainLayer *s_sb_layer;

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_24_bold;
static TextLayer *s_station_name;
static InverterLayer *s_inverterlayer_2;
static InverterLayer *s_inverterlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  // s_station_name
  s_station_name = text_layer_create(GRect(0, -7, 144, 29));
  text_layer_set_background_color(s_station_name, GColorClear);
  text_layer_set_text(s_station_name, "Mountain View");
  text_layer_set_text_alignment(s_station_name, GTextAlignmentCenter);
  text_layer_set_font(s_station_name, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_station_name);
  
  // s_inverterlayer_2
  s_inverterlayer_2 = inverter_layer_create(GRect(0, 0, 145, 21));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_2);
  
  // s_inverterlayer_1
  s_inverterlayer_1 = inverter_layer_create(GRect(0, 86, 144, 2));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_station_name);
  inverter_layer_destroy(s_inverterlayer_2);
  inverter_layer_destroy(s_inverterlayer_1);
}
// END AUTO-GENERATED UI CODE

static void prv_init_custom_ui(void) {
  s_nb_layer = next_train_layer_create(GRect(0, 14, 144, 71), "NB");
  s_sb_layer = next_train_layer_create(GRect(0, 80, 144, 71), "SB");
  
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_nb_layer);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sb_layer);
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
  window_stack_push(s_window, true);
  persist_showing_stop(stop_id);
}

void hide_stop_info(void) {
  window_stack_remove(s_window, true);
}
