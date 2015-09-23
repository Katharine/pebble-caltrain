#ifdef PBL_ROUND

#include "trip_stop_list.h"
#include <pebble.h>
#include "model.h"
#include "time_utils.h"
#include "colours.h"

static Window *s_window;
static uint16_t s_trip_id;
static TrainTrip s_trip;
static uint8_t s_sequence;
static uint8_t s_time_count;
static TrainTime *s_times;
static uint8_t s_selected_stop;

static TextLayer *s_stop_layer;
// static StatusBarLayer *s_status_layer;
static TextLayer *s_zone_layer;
static char s_zone_buf[7];
static TextLayer *s_time_layer;
static char s_time_buf[17];

static const int16_t ARC_WIDTH = 4;

static inline uint8_t prv_get_stop_id(uint8_t i) {
  return (s_sequence + i ) % s_time_count;
}

static inline GColor prv_color_stop(uint8_t stop_id) {
  return stop_id  >= s_sequence ? COLOUR_ROUTE_FILLED : COLOUR_ROUTE_EMPTY;
}

static void prv_draw_window(Layer *layer, GContext *ctx) {
  const GRect bounds = layer_get_bounds(layer);
  const GRect inset_bounds = grect_inset(bounds, GEdgeInsets(13));
  
  graphics_context_set_fill_color(ctx, COLOUR_WINDOW);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  graphics_context_set_fill_color(ctx, COLOUR_MENU_HIGHLIGHT_BACKGROUND);
  graphics_fill_radial(ctx, grect_inset(bounds, GEdgeInsets(26)), GOvalScaleModeFillCircle, 90, 0, TRIG_MAX_ANGLE);

  int32_t d_theta = TRIG_MAX_ANGLE / s_time_count;
  // int32_t theta = 0;
  graphics_context_set_stroke_width(ctx, ARC_WIDTH);
  for(uint8_t i = 0; i < s_time_count; i++) {
    int32_t theta = i * d_theta;
    uint8_t stop_id = prv_get_stop_id(i);
    graphics_context_set_stroke_color(ctx, prv_color_stop(stop_id));
    if (stop_id != s_time_count - 1) {
      graphics_draw_arc(ctx, inset_bounds, GOvalScaleModeFillCircle, theta, theta + d_theta);
    }
  }

  for(uint8_t i = 0; i < s_time_count; i++) {
    int32_t theta = i * d_theta;
    uint8_t stop_id =  prv_get_stop_id(i);
    GPoint xy = gpoint_from_polar(inset_bounds, GOvalScaleModeFillCircle, theta);
    graphics_context_set_fill_color(ctx, stop_id  == s_selected_stop 
                                    ? COLOUR_MENU_HIGHLIGHT_BACKGROUND : prv_color_stop(stop_id));
    graphics_fill_circle(ctx, xy, 5);
  }
}

static uint8_t prv_sequence_to_index(uint8_t sequence) {
  if(s_trip.direction == TrainDirectionSouthbound) {
    return sequence;
  } else {
    return s_time_count - 1 - sequence;
  }
}

static void prv_update_ui(void) {
  static TrainStop stop;
  TrainTime *time = &s_times[prv_sequence_to_index(s_selected_stop)];
  stop_get(time->stop, &stop);

  snprintf(s_zone_buf, sizeof(s_zone_buf), "Zone %d", stop.zone);
  char minutes_buf[6];
  train_time_format_minutes(time->time, sizeof(minutes_buf), minutes_buf);
  int16_t time_diff = time->time - s_times[prv_sequence_to_index(s_sequence)].time;

  if(time_diff > 0) {
    snprintf(s_time_buf, sizeof(s_time_buf), "%s (%d min)", minutes_buf, time_diff);
  } else {
    strncpy(s_time_buf, minutes_buf, sizeof(s_time_buf));
  }


  text_layer_set_text(s_stop_layer, stop.name);
  text_layer_set_text(s_time_layer, s_time_buf);
  text_layer_set_text(s_zone_layer, s_zone_buf);
}

static void prv_handle_click(ClickRecognizerRef recognizer, void *context) {
  ButtonId button = click_recognizer_get_button_id(recognizer);
  int8_t direction = (button == BUTTON_ID_DOWN) ? 1 : -1;
  int8_t next_stop = s_selected_stop + direction;
  if (next_stop < 0 || next_stop >= s_time_count) {
    return;
  }
  s_selected_stop = next_stop;
  prv_update_ui();
}

static void prv_click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, prv_handle_click);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, prv_handle_click);
}

static void prv_init_ui(void) {
	s_window = window_create();
	window_set_background_color(s_window, COLOUR_WINDOW);
	window_stack_push(s_window, true);
  window_set_click_config_provider(s_window, prv_click_config_provider);
	layer_set_update_proc(window_get_root_layer(s_window), prv_draw_window);

  // s_status_layer = status_bar_layer_create();
  // status_bar_layer_set_colors(s_status_layer, GColorClear, GColorBlack);
  // layer_set_frame((Layer *)s_status_layer, GRect(0, 15, 180, 20));
  // layer_add_child(window_get_root_layer(s_window), (Layer *)s_status_layer);

  s_stop_layer = text_layer_create(GRect(30, 70, 120, 30));
  text_layer_set_font(s_stop_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(s_stop_layer, GColorWhite);
  text_layer_set_background_color(s_stop_layer, GColorClear);
  text_layer_set_text_alignment(s_stop_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_stop_layer);

  s_time_layer = text_layer_create(GRect(30, 100, 120, 30));
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time_layer);

  s_zone_layer = text_layer_create(GRect(30, 50, 120, 30));
  text_layer_set_font(s_zone_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_zone_layer, GColorWhite);
  text_layer_set_background_color(s_zone_layer, GColorClear);
  text_layer_set_text_alignment(s_zone_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_zone_layer);

  prv_update_ui();
}

static void prv_deinit_ui(void) {
	window_stack_pop(s_window);
	window_destroy(s_window);
  text_layer_destroy(s_stop_layer);
  text_layer_destroy(s_zone_layer);
  text_layer_destroy(s_time_layer);
}

void show_trip_stop_list(uint16_t trip_id, uint8_t sequence) {
	s_trip_id = trip_id;
	s_sequence = sequence - 1;
  trip_get(trip_id, &s_trip);
  s_selected_stop = s_sequence;
	s_time_count = trip_times_count(trip_id);
  s_times = malloc(s_time_count * sizeof(TrainTime));
  trip_get_times(trip_id, s_time_count, s_times);
	prv_init_ui();
}

void hide_trip_stop_list(void) {
	prv_deinit_ui();
}

#endif
