#pragma once

void persist_showing_stop(uint8_t stop_id);
void persist_hiding_stop(void);
void persist_selected_stop(uint8_t stop_id);

void restore_state();
