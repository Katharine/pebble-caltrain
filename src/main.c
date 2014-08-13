#include <pebble.h>

#include "model.h"
#include "stop_list.h"
#include "persist.h"
#include "appmessage.h"

void message_received(DictionaryIterator *iter, void *context) {
  Tuple *command_id = dict_find(iter, COMMAND_ID);
  if(!command_id) {
    return;
  }
  if(command_id->value->int32 == 1) {
    show_nearest_stop(dict_find(iter, POS_LON)->value->int32, dict_find(iter, POS_LAT)->value->int32);
  }
}

int main(void) {
  show_stop_list();
  restore_state();
  app_message_register_inbox_received(message_received);
  app_message_open(64, 64);
  app_event_loop();
  return 0;
}
