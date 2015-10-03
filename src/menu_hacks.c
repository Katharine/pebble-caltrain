#include <pebble.h>
#include "menu_hacks.h"
#include "colours.h"

void menu_set_colours(MenuLayer *menu) {
  #ifdef PBL_COLOR
    menu_layer_set_normal_colors(menu, COLOUR_MENU_BACKGROUND, COLOUR_MENU_FOREGROUND);
    menu_layer_set_highlight_colors(menu, COLOUR_MENU_HIGHLIGHT_BACKGROUND, COLOUR_MENU_HIGHLIGHT_FOREGROUND);
  #endif
}

int16_t menu_hack_borderless_cells(struct MenuLayer *menu_layer, MenuIndex *cell_index,
                                   void *callback_context) {
  if (watch_info_get_firmware_version().major >= 3) {
    return 0;
  } else {
    return 1;
  }
}
