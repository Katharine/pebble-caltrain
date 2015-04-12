#include <pebble.h>
#include "menu_hacks.h"
#include "colours.h"
  
void menu_hack_set_colours(GContext *ctx, MenuLayer *menu, MenuIndex *index) {
  #ifdef PBL_COLOR
  if(menu_layer_get_selected_index(menu).row == index->row) {
    graphics_context_set_fill_color(ctx, COLOUR_MENU_HIGHLIGHT_BACKGROUND);
    graphics_context_set_text_color(ctx, COLOUR_MENU_HIGHLIGHT_FOREGROUND);
  } else {
    graphics_context_set_fill_color(ctx, COLOUR_MENU_BACKGROUND);
    graphics_context_set_text_color(ctx, COLOUR_MENU_FOREGROUND);
  }
  #else
    graphics_context_set_fill_color(ctx, COLOUR_MENU_BACKGROUND);
    graphics_context_set_text_color(ctx, COLOUR_MENU_FOREGROUND);
  #endif
}

void menu_hack_disable_inversion(MenuLayer *menu) {
  WatchInfoVersion fw_version = watch_info_get_firmware_version();
  if(fw_version.major == 3 && fw_version.minor == 0 && fw_version.patch == 0) {
//     ((uint32_t *)menu)[46] = 0;
  } else if(fw_version.major == 2 && fw_version.minor == 9) {
    ((uint32_t *)menu)[60] = 0;
  }
}

int16_t menu_hack_borderless_cells(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if(watch_info_get_firmware_version().major >= 3) {
    return 0;
  } else {
    return 1;
  }
}