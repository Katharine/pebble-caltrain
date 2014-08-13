#include "stop_info.h"
#include <pebble.h>

#include "model.h"
#include "planning.h"
#include "persist.h"

static uint8_t s_stop_id;
static TrainStop s_stop;
static TrainTime s_northbound;
static TrainTime s_southbound;
static char s_sb_time_text[] = "00:00";
static char s_nb_time_text[] = "00:00";
static char s_nb_number_text[] = "123";
static char s_sb_number_text[] = "123";
static char s_nb_state_text[] = "100 min late";
static char s_sb_state_text[] = "100 min late";

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_28_bold;
static GFont s_res_bitham_42_medium_numbers;
static GFont s_res_gothic_28;
static GFont s_res_gothic_24;
static TextLayer *s_station_name;
static InverterLayer *s_inverterlayer_2;
static InverterLayer *s_inverterlayer_1;
static TextLayer *s_textlayer_1;
static TextLayer *s_sb_next_time;
static TextLayer *s_sb_status;
static TextLayer *s_textlayer_4;
static TextLayer *s_nb_next_time;
static TextLayer *s_nb_status;
static TextLayer *s_sb_number;
static TextLayer *s_nb_number;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_bitham_42_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
  s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);
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
  s_inverterlayer_1 = inverter_layer_create(GRect(0, 83, 144, 2));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(2, 18, 24, 28));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text(s_textlayer_1, "SB");
  text_layer_set_font(s_textlayer_1, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  // s_sb_next_time
  s_sb_next_time = text_layer_create(GRect(21, 15, 121, 43));
  text_layer_set_background_color(s_sb_next_time, GColorClear);
  text_layer_set_text(s_sb_next_time, "00:00");
  text_layer_set_text_alignment(s_sb_next_time, GTextAlignmentRight);
  text_layer_set_font(s_sb_next_time, s_res_bitham_42_medium_numbers);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sb_next_time);
  
  // s_sb_status
  s_sb_status = text_layer_create(GRect(30, 52, 114, 32));
  text_layer_set_background_color(s_sb_status, GColorClear);
  text_layer_set_text(s_sb_status, "4 min");
  text_layer_set_text_alignment(s_sb_status, GTextAlignmentRight);
  text_layer_set_font(s_sb_status, s_res_gothic_28);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sb_status);
  
  // s_textlayer_4
  s_textlayer_4 = text_layer_create(GRect(2, 83, 25, 28));
  text_layer_set_background_color(s_textlayer_4, GColorClear);
  text_layer_set_text(s_textlayer_4, "NB");
  text_layer_set_font(s_textlayer_4, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_4);
  
  // s_nb_next_time
  s_nb_next_time = text_layer_create(GRect(21, 80, 121, 42));
  text_layer_set_background_color(s_nb_next_time, GColorClear);
  text_layer_set_text(s_nb_next_time, "00:00");
  text_layer_set_text_alignment(s_nb_next_time, GTextAlignmentRight);
  text_layer_set_font(s_nb_next_time, s_res_bitham_42_medium_numbers);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_nb_next_time);
  
  // s_nb_status
  s_nb_status = text_layer_create(GRect(29, 120, 115, 32));
  text_layer_set_background_color(s_nb_status, GColorClear);
  text_layer_set_text(s_nb_status, "Arriving");
  text_layer_set_text_alignment(s_nb_status, GTextAlignmentRight);
  text_layer_set_font(s_nb_status, s_res_gothic_28);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_nb_status);
  
  // s_sb_number
  s_sb_number = text_layer_create(GRect(0, 56, 39, 24));
  text_layer_set_background_color(s_sb_number, GColorClear);
  text_layer_set_text(s_sb_number, "157");
  text_layer_set_font(s_sb_number, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sb_number);
  
  // s_nb_number
  s_nb_number = text_layer_create(GRect(0, 124, 34, 24));
  text_layer_set_background_color(s_nb_number, GColorClear);
  text_layer_set_text(s_nb_number, "132");
  text_layer_set_font(s_nb_number, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_nb_number);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_station_name);
  inverter_layer_destroy(s_inverterlayer_2);
  inverter_layer_destroy(s_inverterlayer_1);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_sb_next_time);
  text_layer_destroy(s_sb_status);
  text_layer_destroy(s_textlayer_4);
  text_layer_destroy(s_nb_next_time);
  text_layer_destroy(s_nb_status);
  text_layer_destroy(s_sb_number);
  text_layer_destroy(s_nb_number);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
  persist_hiding_stop();
}

static void format_minutes(uint16_t minutes, char* buffer) {
  minutes = minutes % (60 * 24);
  snprintf(buffer, 6, "%02d:%02d", (int)(minutes / 60), (int)(minutes % 60));
}

static void format_state(TrainTime *train_time, size_t n, char* buffer) {
  uint16_t now = current_minute();
  int16_t diff = train_time->time - now;
  if(diff > 0) {
    snprintf(buffer, n, "%d min", diff);
  } else if(diff == 0) {
    strncpy(buffer, "Arriving", n);
  } else {
    strncpy(buffer, "", n);
  }
}

static void update_states(void) {
  format_state(&s_northbound, sizeof(s_nb_state_text), s_nb_state_text);
  format_state(&s_southbound, sizeof(s_sb_state_text), s_sb_state_text);
  text_layer_set_text(s_nb_status, s_nb_state_text);
  text_layer_set_text(s_sb_status, s_sb_state_text);
}

static void update_stop_ui(void) {
  next_train_at_station(s_stop_id, &s_northbound, &s_southbound);
  text_layer_set_text(s_station_name, s_stop.name);
  format_minutes(s_northbound.time, s_nb_time_text);
  format_minutes(s_southbound.time, s_sb_time_text);
  text_layer_set_text(s_nb_next_time, s_nb_time_text);
  text_layer_set_text(s_sb_next_time, s_sb_time_text);
  
  TrainTrip nb_trip, sb_trip;
  trip_get(s_northbound.trip, &nb_trip);
  trip_get(s_southbound.trip, &sb_trip);
  snprintf(s_nb_number_text, sizeof(s_nb_number_text), "%d", nb_trip.trip_name);
  snprintf(s_sb_number_text, sizeof(s_sb_number_text), "%d", sb_trip.trip_name);
  text_layer_set_text(s_nb_number, s_nb_number_text);
  text_layer_set_text(s_sb_number, s_sb_number_text);
  
  update_states();
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  uint16_t now = current_minute();
  if(now > s_northbound.time || now > s_southbound.time) {
    update_stop_ui();
  } else {
    update_states();
  }
}

void show_stop_info(uint8_t stop_id) {
  if(!stop_get(stop_id, &s_stop)) {
    return;
  }
  s_stop_id = stop_id;
  initialise_ui();
  update_stop_ui();
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
  persist_showing_stop(stop_id);
}

void hide_stop_info(void) {
  window_stack_remove(s_window, true);
}
