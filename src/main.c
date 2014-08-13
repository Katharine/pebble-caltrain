#include <pebble.h>

#include "model.h"
#include "stop_list.h"
#include "persist.h"
  
int main(void) {
  show_stop_list();
  restore_state();
  app_event_loop();
  return 0;
}
