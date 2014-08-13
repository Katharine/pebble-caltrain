#include "stop_info.h"
#include <pebble.h>

#include "model.h"
#include "planning.h"

static uint8_t s_stop_id;
static TrainStop s_stop;
static TrainTime s_northbound;
static TrainTime s_southbound;
static char s_sb_time_text[] = "00:00";
static char s_nb_time_text[] = "00:00";

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_28_bold;
static GFont s_res_bitham_42_medium_numbers;
static GFont s_res_gothic_28;
static TextLayer *s_station_name;
static InverterLayer *s_inverterlayer_2;
static InverterLayer *s_inverterlayer_1;
static TextLayer *s_textlayer_1;
static TextLayer *s_sb_next_time;
static TextLayer *s_sb_status;
static TextLayer *s_textlayer_4;
static TextLayer *s_nb_next_time;
static TextLayer *s_nb_status;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_bitham_42_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
  s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);
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
  s_sb_next_time = text_layer_create(GRect(33, 15, 110, 43));
  text_layer_set_background_color(s_sb_next_time, GColorClear);
  text_layer_set_text(s_sb_next_time, "9:32");
  text_layer_set_font(s_sb_next_time, s_res_bitham_42_medium_numbers);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sb_next_time);
  
  // s_sb_status
  s_sb_status = text_layer_create(GRect(0, 51, 144, 31));
  text_layer_set_background_color(s_sb_status, GColorClear);
  text_layer_set_text(s_sb_status, "19 min away");
  text_layer_set_text_alignment(s_sb_status, GTextAlignmentCenter);
  text_layer_set_font(s_sb_status, s_res_gothic_28);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sb_status);
  
  // s_textlayer_4
  s_textlayer_4 = text_layer_create(GRect(2, 83, 25, 28));
  text_layer_set_background_color(s_textlayer_4, GColorClear);
  text_layer_set_text(s_textlayer_4, "NB");
  text_layer_set_font(s_textlayer_4, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_4);
  
  // s_nb_next_time
  s_nb_next_time = text_layer_create(GRect(33, 80, 111, 42));
  text_layer_set_background_color(s_nb_next_time, GColorClear);
  text_layer_set_text(s_nb_next_time, "9:59");
  text_layer_set_font(s_nb_next_time, s_res_bitham_42_medium_numbers);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_nb_next_time);
  
  // s_nb_status
  s_nb_status = text_layer_create(GRect(0, 118, 144, 32));
  text_layer_set_background_color(s_nb_status, GColorClear);
  text_layer_set_text(s_nb_status, "Arriving");
  text_layer_set_text_alignment(s_nb_status, GTextAlignmentCenter);
  text_layer_set_font(s_nb_status, s_res_gothic_28);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_nb_status);
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
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

static void format_minutes(uint16_t minutes, char* buffer) {
  minutes = minutes % (60 * 24);
  snprintf(buffer, 6, "%02d:%02d", (int)(minutes / 60), (int)(minutes % 60));
}

static void update_stop_ui(void) {
  text_layer_set_text(s_station_name, s_stop.name);
  next_train_at_station(s_stop_id, &s_northbound, &s_southbound);
  format_minutes(s_northbound.time, s_nb_time_text);
  format_minutes(s_southbound.time, s_sb_time_text);
  text_layer_set_text(s_nb_next_time, s_nb_time_text);
  text_layer_set_text(s_sb_next_time, s_sb_time_text);
}

void show_stop_info(uint8_t stop_id) {
  if(!stop_get(stop_id, &s_stop)) {
    return;
  }
  s_stop_id = stop_id;
  initialise_ui();
  update_stop_ui();
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_stop_info(void) {
  window_stack_remove(s_window, true);
}
