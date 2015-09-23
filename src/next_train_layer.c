#include <pebble.h>
#include "next_train_layer.h"
#include "model.h"
#include "time_utils.h"

typedef struct {
  TextLayer *direction_layer;
  TextLayer *number_layer;
  TextLayer *state_layer;
  TextLayer *time_layer;
  NextTrainLayerStyle style;
#ifdef PBL_COLOR
  Layer *colour_layer;
  GColor colour_layer_colour;
#endif
  char time_text[6];
  char number_text[4];
  char state_text[13];
} NextTrainLayerData;

typedef struct {
  GRect direction;
  GRect number;
  char *number_font;
  GRect state;
  char *state_font;
  GRect time;
  char *time_font;
  GPoint dot_pos;
  int16_t dot_radius;
  GTextAlignment alignment;
} NextTrainLayout;

static const NextTrainLayout LAYOUTS[NextTrainLayerStyleNumStyles] = {
#ifndef PBL_ROUND
  [NextTrainLayerStyleHorizontal] = {
    {{2, 3}, {24, 28}},
    {{0, 45}, {39, 24}},
    FONT_KEY_GOTHIC_24,
    {{30, 41}, {114, 32}},
    FONT_KEY_GOTHIC_28,
    {{21, 0}, {121, 43}},
    FONT_KEY_BITHAM_42_MEDIUM_NUMBERS,
    {8, 43},
    5,
    GTextAlignmentRight,
  },
#else
  [NextTrainLayerStyleRightAligned] = {
    {{5, 0}, {24, 28}},
    {{38, 0}, {39, 32}},
    FONT_KEY_GOTHIC_28,
    {{0, 60}, {74, 64}},
    FONT_KEY_GOTHIC_24,
    {{0, 30}, {74, 34}},
    FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM,
    {60, 100},
    9,
    GTextAlignmentRight,
  },
  [NextTrainLayerStyleLeftAligned] = {
    {{48, 0}, {24, 28}},
    {{0, 0}, {39, 32}},
    FONT_KEY_GOTHIC_28,
    {{0, 60}, {74, 64}},
    FONT_KEY_GOTHIC_24,
    {{0, 30}, {74, 34}},
    FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM,
    {11, 100},
    9,
    GTextAlignmentLeft,
  },
#endif
};

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
  graphics_fill_circle(ctx, LAYOUTS[data->style].dot_pos, LAYOUTS[data->style].dot_radius);
  // graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorRed, GColorWhite));
  // graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}
#endif

NextTrainLayer *next_train_layer_create(GRect frame, const char *direction_name, NextTrainLayerStyle style) {
  Layer *layer = layer_create_with_data(frame, sizeof(NextTrainLayerData));
  NextTrainLayerData *data = layer_get_data(layer);
  data->style = style;
  const NextTrainLayout *layout = &LAYOUTS[style];
  
  data->direction_layer = text_layer_create(layout->direction);
  text_layer_set_font(data->direction_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_background_color(data->direction_layer, GColorClear);
  text_layer_set_text(data->direction_layer, direction_name);
  layer_add_child(layer, (Layer *)data->direction_layer);
  
  data->number_layer = text_layer_create(layout->number);
  text_layer_set_font(data->number_layer, fonts_get_system_font(layout->number_font));
  text_layer_set_background_color(data->number_layer, GColorClear);
  layer_add_child(layer, (Layer *)data->number_layer);
  
  data->state_layer = text_layer_create(layout->state);
  text_layer_set_background_color(data->state_layer, GColorClear);
  text_layer_set_text_alignment(data->state_layer, layout->alignment);
  text_layer_set_font(data->state_layer, fonts_get_system_font(layout->state_font));
  layer_add_child(layer, (Layer *)data->state_layer);
  
  data->time_layer = text_layer_create(layout->time);
  text_layer_set_background_color(data->time_layer, GColorClear);
  text_layer_set_text_alignment(data->time_layer, layout->alignment);
  text_layer_set_font(data->time_layer, fonts_get_system_font(layout->time_font));
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
