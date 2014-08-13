#include "stop_list.h"
#include <pebble.h>

#include "model.h"
#include "stop_info.h"
#include "persist.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_menu;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  // s_menu
  s_menu = menu_layer_create(GRect(0, 0, 144, 152));
  menu_layer_set_click_config_onto_window(s_menu, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menu);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_menu);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  persist_selected_stop(menu_layer_get_selected_index(s_menu).row);
  destroy_ui();
}

static void draw_menu_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  TrainStop stop;
  stop_get(cell_index->row, &stop);
  char zone[] = "zone 50";
  snprintf(zone, sizeof(zone), "Zone %d", (int)stop.zone);
  menu_cell_basic_draw(ctx, cell_layer, stop.name, zone, NULL);
}

static uint16_t get_menu_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return stop_count();
}

static void handle_menu_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  show_stop_info(cell_index->row);
}

void select_list_select_stop(uint8_t stop_id, bool show_window) {
  menu_layer_set_selected_index(s_menu, (MenuIndex){.section=0, .row=stop_id}, MenuRowAlignCenter, false);
  if(show_window) {
    show_stop_info(stop_id);
  }
}

void show_stop_list(void) {
  initialise_ui();
  
  menu_layer_set_callbacks(s_menu, NULL, (MenuLayerCallbacks){
    .draw_row = draw_menu_row,
    .get_num_rows = get_menu_rows,
    .select_click = handle_menu_click,
  });
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_stop_list(void) {
  window_stack_remove(s_window, true);
}
