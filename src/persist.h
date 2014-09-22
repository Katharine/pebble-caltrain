#pragma once

// Record that we are currently showing stop_id
void persist_showing_stop(uint8_t stop_id);
// Record that we are no longer showing a stop.
void persist_hiding_stop(void);
// Record that stop_id is the currently selected stop.
void persist_selected_stop(uint8_t stop_id);

// Restore the persisted UI state.
void restore_state();
