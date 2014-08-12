#include <pebble.h>

#include "model.h"
#include "stop_list.h"
  
int main(void) {
  show_stop_list();
  app_event_loop();
  return 0;
}
