#include <pebble.h>
#include "menu_hacks.h"
#include "colours.h"
  
void menu_set_colours(MenuLayer *menu) {
  #ifdef PBL_COLOR
    menu_layer_set_normal_colors(menu, COLOUR_MENU_BACKGROUND, COLOUR_MENU_FOREGROUND);
    menu_layer_set_highlight_colors(menu, COLOUR_MENU_HIGHLIGHT_BACKGROUND, COLOUR_MENU_HIGHLIGHT_FOREGROUND);
  #endif
}
