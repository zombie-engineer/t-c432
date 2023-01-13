#pragma once
#include <stdint.h>
#include <stdbool.h>

void ssd1306_init(void);

bool dbuf_get_frame_size(int *size_x, int *size_y);
int dbuf_get_pixel(int x, int y);
void dbuf_draw_rect(int x0, int y0, int x1, int y1, int color);
void dbuf_draw_filled_rect(int x0, int y0, int x1, int y1, int color);
void dbuf_draw_hatched_rect(int x0, int y0, int x1, int y1, int color);
void dbuf_draw_line(int x0, int y0, int x1, int y1, int color);
void dbuf_draw_pixel(int x, int y, int color);
void dbuf_clear(void);
void dbuf_flush(void);

struct font_descriptor;
void dbuf_draw_char(int *x, int y, char c, const struct font_descriptor *f, int color);
int dbuf_draw_text(int x, int y, const char *text, const struct font_descriptor *f, int color);

bool dbuf_get_text_size(
  const char *text,
  const struct font_descriptor *f,
  int *size_x,
  int *size_y,
  bool with_spacing);
