#include <pebble.h>
#include "persist.h"
#include "stop_list.h"

#define STATE_PERSIST_KEY 1
  
typedef struct {
  uint8_t stop_id;
  bool window_open;
} AppState;

static AppState s_state;

// Actually write the state to persistent storage.
static void prv_persist_state(void) {
  persist_write_data(STATE_PERSIST_KEY, &s_state, sizeof(s_state));
}

void persist_showing_stop(uint8_t stop_id) {
  s_state.stop_id = stop_id;
  s_state.window_open = true;
  prv_persist_state();
}

void persist_hiding_stop(void) {
  s_state.window_open = false;
  prv_persist_state();
}

void persist_selected_stop(uint8_t stop_id) {
  if(!s_state.window_open) {
    s_state.stop_id = stop_id;
    prv_persist_state();
  }
}

void restore_state() {
  memset(&s_state, 0, sizeof(s_state));
  if(persist_exists(STATE_PERSIST_KEY)) {
    persist_read_data(STATE_PERSIST_KEY, &s_state, sizeof(s_state));
    select_list_select_stop(s_state.stop_id, s_state.window_open);
  }
}
