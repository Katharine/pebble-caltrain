#include "train_times_list.h"
#include <pebble.h>
#include "model.h"
#include "planning.h"
#include "time_utils.h"
#include "trip_stop_list.h"
#include "colours.h"
#include "menu_hacks.h"

static TrainDirection s_direction;
static uint8_t s_stop_id;
static TrainStop s_stop;
static TrainTime *s_times;
static uint16_t s_time_count;

static const char *s_direction_names[2] = {"Southbound", "Northbound"};

#ifdef PBL_ROUND
static StatusBarLayer *s_status_bar;
#endif

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_18;
static InverterLayer *s_inverterlayer_1;
static TextLayer *s_stop_name_layer;
static TextLayer *s_direction_layer;
static MenuLayer *s_train_menu;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, PBL_IF_RECT_ELSE(false, true));
  window_set_background_color(s_window, COLOUR_WINDOW);
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  // s_inverterlayer_1
  s_inverterlayer_1 = inverter_layer_create(GRect(0, 0, 144, 35));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
  
  // s_direction_layer
  s_direction_layer = text_layer_create(GRect(0, 14, 144, 21));
  text_layer_set_background_color(s_direction_layer, COLOUR_HEADER);
  text_layer_set_text_color(s_direction_layer, COLOUR_HEADER_TEXT);
  text_layer_set_text(s_direction_layer, "Southbound");
  text_layer_set_text_alignment(s_direction_layer, GTextAlignmentCenter);
  text_layer_set_font(s_direction_layer, s_res_gothic_18);
  #ifndef PBL_ROUND
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_direction_layer);
  #endif
  
  // s_stop_name_layer
  s_stop_name_layer = text_layer_create(GRect(0, -8, 144, 28));
  text_layer_set_background_color(s_stop_name_layer, COLOUR_HEADER);
  text_layer_set_text_color(s_stop_name_layer, COLOUR_HEADER_TEXT);
  text_layer_set_text(s_stop_name_layer, "Mountain View");
  text_layer_set_text_alignment(s_stop_name_layer, GTextAlignmentCenter);
  text_layer_set_font(s_stop_name_layer, s_res_gothic_24_bold);
  #ifndef PBL_ROUND
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_stop_name_layer);
  #endif
  
  // s_train_menu
  GSize size = layer_get_bounds(window_get_root_layer(s_window)).size;
  s_train_menu = menu_layer_create(GRect(0, PBL_IF_RECT_ELSE(35, 0), size.w, size.h - PBL_IF_RECT_ELSE(35, 0)));
  #ifdef PBL_ROUND
    menu_layer_set_center_focused(s_train_menu, true);
  #endif
  menu_layer_set_click_config_onto_window(s_train_menu, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_train_menu);

  #ifdef PBL_ROUND
  s_status_bar = status_bar_layer_create();
  status_bar_layer_set_colors(s_status_bar, GColorWhite, GColorBlack);
  status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeDotted);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_status_bar);
  #endif
}

static void destroy_ui(void) {
  window_destroy(s_window);
  inverter_layer_destroy(s_inverterlayer_1);
  text_layer_destroy(s_stop_name_layer);
  text_layer_destroy(s_direction_layer);
  menu_layer_destroy(s_train_menu);
}
// END AUTO-GENERATED UI CODE

static void prv_handle_window_unload(Window* window) {
  destroy_ui();
  free(s_times);
}

static void prv_draw_menu_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *menu) {
  #ifdef PBL_SDK_2
  graphics_context_set_text_color(ctx, COLOUR_MENU_FOREGROUND);
  #endif
  uint16_t row = cell_index->row;
  #ifdef PBL_ROUND
  if (row == 0) {
    graphics_draw_text(ctx, s_stop.name, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(0, -2, 180, 30), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, s_direction_names[s_direction], fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(0, 20, 180, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    return;
  } else {
    row -= 1;
  }
  #endif
  char time_buf[6];
  char state_buf[13];
  char number_buf[4];
  TrainTrip trip;
  
  TrainTime *time = &s_times[row];
  
  trip_get(time->trip, &trip);
  snprintf(number_buf, sizeof(number_buf), "%d", trip.trip_name);
  train_time_format_minutes(time->time, sizeof(time_buf), time_buf);
  train_time_format_state(time, sizeof(state_buf), state_buf);

  #ifndef PBL_SDK_2
  const bool highlighted = menu_cell_layer_is_highlighted(cell_layer);
  #endif
  
  GRect time_rect = GRect(0, -5, layer_get_bounds(cell_layer).size.w, 43);
  #ifdef PBL_ROUND
    if(highlighted) {
      time_rect.origin.y += 5;
    }
  #endif
  graphics_draw_text(ctx, time_buf, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS), time_rect, 
                     GTextOverflowModeFill, PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter), NULL);

  GPoint circle_point = PBL_IF_RECT_ELSE(GPoint(135, 26), GPoint(105, 49));

  #ifndef PBL_ROUND
    graphics_draw_text(ctx, number_buf, fonts_get_system_font(FONT_KEY_GOTHIC_24), GRect(114, -6, 27, 20), GTextOverflowModeFill, GTextAlignmentRight, NULL);
  #else
    if(highlighted) {
      graphics_draw_text(ctx, number_buf, fonts_get_system_font(FONT_KEY_GOTHIC_24), GRect(0, 32, 98, 20), GTextOverflowModeFill, GTextAlignmentRight, NULL);
      graphics_context_set_fill_color(ctx, trip_get_colour(&trip));
    }
  #endif
  
  #ifdef PBL_COLOR
    if(PBL_IF_RECT_ELSE(true, highlighted)) {
      if (gcolor_equal(trip_get_colour(&trip), COLOUR_MENU_HIGHLIGHT_BACKGROUND)) {
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_draw_circle(ctx, circle_point, 5);
      } else {
        graphics_context_set_fill_color(ctx, trip_get_colour(&trip));
        graphics_fill_circle(ctx, circle_point, 5);
      }
    }
  #endif
}

static uint16_t prv_get_menu_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return s_time_count + PBL_IF_RECT_ELSE(0, 1);
}

static void prv_handle_menu_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  int16_t row = cell_index->row;
  #ifdef PBL_ROUND
  if (row == 0) {
    return;
  }
  row -= 1;
  #endif
  if(row < s_time_count) {
    const TrainTime *time = &s_times[row];
    show_trip_stop_list(time->trip, time->sequence);
  }
}

static int16_t prv_get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  MenuIndex selected_index = menu_layer_get_selected_index(menu_layer);
  #ifdef PBL_ROUND
    if (cell_index->row == 0) {
      return 45;
    }
  #endif
  return PBL_IF_RECT_ELSE(36, menu_index_compare(&selected_index, cell_index) == 0 ? 66 : 36);
}

static void prv_init_custom_ui(void) {
  menu_layer_set_callbacks(s_train_menu, s_train_menu, (MenuLayerCallbacks) {
    .draw_row = prv_draw_menu_row,
    .get_num_rows = prv_get_menu_rows,
    .select_click = prv_handle_menu_click,
    .get_cell_height = prv_get_cell_height,
  });
  menu_set_colours(s_train_menu);
  stop_get(s_stop_id, &s_stop);
  text_layer_set_text(s_stop_name_layer, s_stop.name);
  text_layer_set_text(s_direction_layer, s_direction_names[s_direction]);
}

void show_train_times_list(uint8_t station, TrainDirection direction) {
  s_stop_id = station;
  s_direction = direction;
  s_time_count = get_future_trains(s_stop_id, s_direction, &s_times);
  initialise_ui();
  prv_init_custom_ui();
  #ifdef PBL_ROUND
  menu_layer_set_selected_index(s_train_menu, (MenuIndex){0, 1}, MenuRowAlignCenter, false);
  #endif
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = prv_handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_train_times_list(void) {
  window_stack_remove(s_window, true);
}
