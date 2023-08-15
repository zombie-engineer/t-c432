#pragma once
#include <stdint.h>
#include "temp_info.h"

void ui_set_temp_history(const struct temp_info *temp_history, int depth);
void ui_set_temperatures(float t0, float t1, float t_int);
void ui_init(void);
void ui_tick(int ms);
void ui_redraw(void);

void ui_on_set_address(void);
void ui_on_tx(const char *buf);
void ui_on_rx(const char *buf);
