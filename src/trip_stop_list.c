#ifndef PBL_ROUND

#include "trip_stop_list.h"
#include <pebble.h>
#include "model.h"
#include "time_utils.h"
#include "colours.h"
#include "menu_hacks.h"

static uint16_t s_trip_id;
static TrainTrip s_trip;
static uint8_t s_index;
static uint8_t s_sequence;
static TrainTime *s_times;
static uint8_t s_time_count;

static char s_trip_name_buf[5];

static Window *s_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_24;
static TextLayer *s_statusbar;
static TextLayer *s_train_number;
static TextLayer *s_train_type;
static MenuLayer *s_stop_list;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  window_set_background_color(s_window, COLOUR_WINDOW);
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  // s_statusbar
  s_statusbar = text_layer_create(GRect(0, 0, 144, 21));
  text_layer_set_background_color(s_statusbar, COLOUR_HEADER);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_statusbar);
  
  // s_train_number
  s_train_number = text_layer_create(GRect(2, -7, 56, 29));
  text_layer_set_background_color(s_train_number, GColorClear);
  text_layer_set_text_color(s_train_number, COLOUR_HEADER_TEXT);
  text_layer_set_text(s_train_number, "#307");
  text_layer_set_font(s_train_number, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_train_number);
  
  // s_train_type
  s_train_type = text_layer_create(GRect(62, -7, 80, 28));
  text_layer_set_background_color(s_train_type, GColorClear);
  text_layer_set_text_color(s_train_type, COLOUR_HEADER_TEXT);
  text_layer_set_text(s_train_type, "Something");
  text_layer_set_text_alignment(s_train_type, GTextAlignmentRight);
  text_layer_set_font(s_train_type, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_train_type);
  
  // s_stop_list
  s_stop_list = menu_layer_create(GRect(0, 21, 144, 131));
  menu_set_colours(s_stop_list);
  if(watch_info_get_firmware_version().major >= 3) {
    scroll_layer_set_shadow_hidden(menu_layer_get_scroll_layer(s_stop_list), true);
  }
  menu_layer_set_click_config_onto_window(s_stop_list, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_stop_list);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_statusbar);
  text_layer_destroy(s_train_number);
  text_layer_destroy(s_train_type);
  menu_layer_destroy(s_stop_list);
}

static uint8_t prv_sequence_to_index(uint8_t sequence) {
  if(s_trip.direction == TrainDirectionSouthbound) {
    return sequence - 1;
  } else {
    return s_time_count - sequence;
  }
}

static void prv_draw_menu_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *menu) {
  #ifndef PBL_COLOR
    graphics_context_set_fill_color(ctx, COLOUR_MENU_BACKGROUND);
    graphics_context_set_text_color(ctx, COLOUR_MENU_FOREGROUND);
  #endif
  char time_buf[17];
  char minutes_buf[6];
  char zone_buf[7];
  TrainStop stop;
  
  TrainTime *time = &s_times[cell_index->row];
  
  stop_get(time->stop, &stop);
  snprintf(zone_buf, sizeof(zone_buf), "Zone %d", stop.zone);
  train_time_format_minutes(time->time, sizeof(minutes_buf), minutes_buf);
  int16_t time_diff = time->time - s_times[s_index].time;
  if(time_diff > 0) {
    snprintf(time_buf, sizeof(time_buf), "%s (%d min)", minutes_buf, time_diff);
  } else {
    strncpy(time_buf, minutes_buf, sizeof(time_buf));
  }
  
  graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
  
  graphics_draw_text(ctx, stop.name, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(13, -7, 131, 29), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, zone_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(14, 16, 129, 20), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, time_buf, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(14, 16, 129, 20), GTextOverflowModeFill, GTextAlignmentRight, NULL);
  
  // Draw in map thing in the left margin.
  const bool is_start = (cell_index->row == 0);
  const bool is_end = (cell_index->row == s_time_count - 1);
  graphics_context_set_fill_color(ctx, COLOUR_ROUTE_FILLED);
  graphics_context_set_stroke_color(ctx, COLOUR_ROUTE_OUTLINE);
  if(time->sequence < s_sequence) { // Empty section because the train has already gone past here.
    int16_t top = -1;
    int16_t bottom = 41;
    // Avoid drawing past either end.
    if(is_start) {
      top = 20;
    } else if(is_end) {
      bottom = 20;
    }
    graphics_context_set_fill_color(ctx, COLOUR_ROUTE_EMPTY);
    graphics_fill_rect(ctx, GRect(3, top, 7, bottom - top), 0, GCornerNone);
    graphics_draw_rect(ctx, GRect(3, top, 7, bottom - top));
    graphics_fill_circle(ctx, GPoint(6, 20), 6); // Fill white and draw black outline to avoid intersecting tracks.
    graphics_draw_circle(ctx, GPoint(6, 20), 6);
  } else if(time->sequence > s_sequence) { // Filled section; we haven't gone here yet.
    GRect rect = GRect(3, -1, 7, 41);
    // Avoid drawing past either end.
    if(is_end) {
      rect.size.h = 21;
    } else if(is_start) {
      rect.size.h = 21;
      rect.origin.y = 20;
    }
    graphics_fill_rect(ctx, rect, 0, GCornerNone);
    graphics_fill_circle(ctx, GPoint(6, 20), 6);
  } else { // Half-filled; we're here, and heading in one direction.
           // Which direction we fill depends on which direction we're going in.
    if(!is_start) {
      if(s_trip.direction == TrainDirectionSouthbound) {
        graphics_context_set_fill_color(ctx, COLOUR_ROUTE_EMPTY);
      } else {
        graphics_context_set_fill_color(ctx, COLOUR_ROUTE_FILLED);
      }
      graphics_fill_rect(ctx, GRect(3, -1, 7, 21), 0, GCornerNone);
      graphics_draw_rect(ctx, GRect(3, -1, 7, 21));
    }
    if(!is_end) {
      if(s_trip.direction == TrainDirectionSouthbound) {
        graphics_context_set_fill_color(ctx, COLOUR_ROUTE_FILLED);
      } else {
        graphics_context_set_fill_color(ctx, COLOUR_ROUTE_EMPTY);
      }
      graphics_fill_rect(ctx, GRect(3, 20, 7, 21), 0, GCornerNone);
      graphics_draw_rect(ctx, GRect(3, -1, 7, 21));
    }
    graphics_context_set_fill_color(ctx, COLOUR_ROUTE_FILLED);
    graphics_fill_circle(ctx, GPoint(6, 20), 6);
    graphics_draw_circle(ctx, GPoint(6, 20), 6);
  }
}

static uint16_t prv_get_menu_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return s_time_count;
}

static int16_t prv_get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  return 40;
}

static void prv_init_custom_ui(uint16_t trip_id, uint8_t sequence) {
  s_time_count = trip_times_count(trip_id);
  s_trip_id = trip_id;
  trip_get(trip_id, &s_trip);
  s_index = prv_sequence_to_index(sequence);
  s_sequence = sequence;
  s_times = malloc(s_time_count * sizeof(TrainTime));
  trip_get_times(trip_id, s_time_count, s_times);
  
  snprintf(s_trip_name_buf, sizeof(s_trip_name_buf), "#%d", s_trip.trip_name);
  text_layer_set_text(s_train_number, s_trip_name_buf);

  static const char *trip_types[] = {
    "Baby Bullet",
    "Limited",
    "Local",
    "Shuttle",
  };
  text_layer_set_text(s_train_type, trip_types[s_trip.route]);
  
  
  // Set up the menu layer
  menu_layer_set_callbacks(s_stop_list, s_stop_list, (MenuLayerCallbacks) {
    .draw_row = prv_draw_menu_row,
    .get_num_rows = prv_get_menu_rows,
    .get_cell_height = prv_get_cell_height,
  });
  const MenuRowAlign align = s_trip.direction == TrainDirectionSouthbound ? MenuRowAlignTop : MenuRowAlignBottom;
  menu_layer_set_selected_index(s_stop_list, (MenuIndex){0, s_index}, align, false);
}

static void prv_destroy_custom_ui(void) {
  free(s_times);
}

static void prv_handle_window_unload(Window* window) {
  prv_destroy_custom_ui();
  destroy_ui();
}

void show_trip_stop_list(uint16_t trip_id, uint8_t sequence) {
  initialise_ui();
  prv_init_custom_ui(trip_id, sequence);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = prv_handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_trip_stop_list(void) {
  window_stack_remove(s_window, true);
}

#endif
