#pragma once

void ui_init(void);
void ui_tick(int ms);
void ui_redraw(void);

void ui_on_set_address(void);
void ui_on_tx(const char *buf);
void ui_on_rx(const char *buf);
