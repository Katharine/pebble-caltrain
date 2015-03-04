#include <pebble.h>
#include "next_train_layer.h"
#include "model.h"
#include "time_utils.h"

typedef struct {
  TextLayer *direction_layer;
  TextLayer *number_layer;
  TextLayer *state_layer;
  TextLayer *time_layer;
#ifdef PBL_COLOR
  Layer *colour_layer;
  GColor colour_layer_colour;
#endif
  char time_text[6];
  char number_text[4];
  char state_text[13];
} NextTrainLayerData;

// Updates the state layer given the TrainTime of the next train.
static void prv_update_states(NextTrainLayer *layer, TrainTime *time) {
  NextTrainLayerData *data = layer_get_data(layer);
  if(time->time == INVALID_TIME) {
    text_layer_set_text(data->state_layer, "No trains.");
  } else {
    train_time_format_state(time, sizeof(data->state_text), data->state_text);
    text_layer_set_text(data->state_layer, data->state_text);
  }
}

#ifdef PBL_COLOR
static void prv_update_proc(Layer *layer, GContext *ctx) {
  NextTrainLayerData *data = layer_get_data(layer);
  graphics_context_set_fill_color(ctx, data->colour_layer_colour);
  graphics_fill_circle(ctx, GPoint(8, 43), 5);
}
#endif

NextTrainLayer *next_train_layer_create(GRect frame, const char *direction_name) {
  Layer *layer = layer_create_with_data(frame, sizeof(NextTrainLayerData));
  NextTrainLayerData *data = layer_get_data(layer);
  
  data->direction_layer = text_layer_create(GRect(2, 3, 24, 28));
  text_layer_set_font(data->direction_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_background_color(data->direction_layer, GColorClear);
  text_layer_set_text(data->direction_layer, direction_name);
  layer_add_child(layer, (Layer *)data->direction_layer);
  
  data->number_layer = text_layer_create(GRect(0, 45, 39, 24));
  text_layer_set_font(data->number_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(data->number_layer, GColorClear);
  layer_add_child(layer, (Layer *)data->number_layer);
  
  data->state_layer = text_layer_create(GRect(30, 41, 114, 32));
  text_layer_set_background_color(data->state_layer, GColorClear);
  text_layer_set_text_alignment(data->state_layer, GTextAlignmentRight);
  text_layer_set_font(data->state_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(layer, (Layer *)data->state_layer);
  
  data->time_layer = text_layer_create(GRect(21, 0, 121, 43));
  text_layer_set_background_color(data->time_layer, GColorClear);
  text_layer_set_text_alignment(data->time_layer, GTextAlignmentRight);
  text_layer_set_font(data->time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  layer_add_child(layer, (Layer *)data->time_layer);
  
  #ifdef PBL_COLOR
    layer_set_update_proc(layer, prv_update_proc);
  #endif
  
  return (NextTrainLayer *)layer;
}

void next_train_layer_destroy(NextTrainLayer *layer) {
  NextTrainLayerData *data = layer_get_data(layer);
  text_layer_destroy(data->direction_layer);
  text_layer_destroy(data->number_layer);
  text_layer_destroy(data->state_layer);
  text_layer_destroy(data->time_layer);
  layer_destroy(layer);
}

void next_train_layer_set_time(NextTrainLayer *layer, TrainTime *time) {
  NextTrainLayerData *data = layer_get_data(layer);
  TrainTrip trip;
  if(time->time == INVALID_TIME) {
    text_layer_set_text(data->time_layer, "--:--");
    text_layer_set_text(data->number_layer, "---");
    #ifdef PBL_COLOR
      data->colour_layer_colour = GColorWhite;
    #endif
  } else {
    train_time_format_minutes(time->time, sizeof(data->time_text), data->time_text);
    text_layer_set_text(data->time_layer, data->time_text);
    trip_get(time->trip, &trip);
    snprintf(data->number_text, sizeof(data->number_text), "%d", trip.trip_name);
    text_layer_set_text(data->number_layer, data->number_text);
    #ifdef PBL_COLOR
      data->colour_layer_colour = trip_get_colour(&trip);
    #endif
  }
  
  prv_update_states(layer, time);
}
