#pragma once
#include <stdint.h>

void ssd1306_init(void);

int dbuf_get_pixel(int x, int y);
void dbuf_draw_line(int x0, int y0, int x1, int y1, int color);
void dbuf_draw_pixel(int x, int y, int color);
void dbuf_clear(void);
void dbuf_flush(void);
