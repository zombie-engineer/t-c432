#include "display.h"
#include "font.h"
#include <stddef.h>
#include <string.h>

static uint16_t display_size_x = 0;
static uint16_t display_size_y = 0;
static uint8_t display_y_per_byte;
static uint8_t *display_buffer = NULL;

#define DBYTE(__col, __page) display_buffer[__page  * display_size_x + __col]

void display_draw_pixel(int x, int y, int color)
{
  uint8_t *p = &DBYTE(x, y / display_y_per_byte);
  int bitidx = y % display_y_per_byte;
  *p = (*p & ~(1<<bitidx)) | (color<<bitidx);
}

void display_draw_filled_rect(int x0, int y0, int x1, int y1, int color)
{
  if (x1 < x0) {
    int tmp = x0;
    x0 = x1;
    x1 = tmp;
  }
  if (y1 < y0) {
    int tmp = y0;
    y0 = y1;
    y1 = tmp;
  }
  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      display_draw_pixel(x, y, color);
    }
  }
}

void display_draw_hatched_rect(int x0, int y0, int x1, int y1, int color)
{
  int p = 0;
  if (x1 < x0) {
    int tmp = x0;
    x0 = x1;
    x1 = tmp;
  }
  if (y1 < y0) {
    int tmp = y0;
    y0 = y1;
    y1 = tmp;
  }
  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      p++;
      display_draw_pixel(x, y, (p % 4) == 0);
    }
  }
}

void display_draw_rect(int x0, int y0, int x1, int y1, int color)
{
  display_draw_line(x0, y0, x0, y1, color);
  display_draw_line(x0, y1, x1, y1, color);
  display_draw_line(x1, y0, x1, y1, color);
  display_draw_line(x0, y0, x1, y0, color);
}

void display_draw_line(int x0, int y0, int x1, int y1, int color)
{
  if (x1 < x0) {
    int tmp = x0;
    x0 = x1;
    x1 = tmp;
  }
  if (y1 < y0) {
    int tmp = y0;
    y0 = y1;
    y1 = tmp;
  }
  if (x1 - x0 > y1 - y0)  {
    float slope = (float)(y1 - y0) / (x1 - x0);
    for (int x = x0; x <= x1; x++) {
      int y = y0 + slope * (x - x0);
      display_draw_pixel(x, y, color);
    }
  } else {
    float slope = (float)(x1 - x0) / (y1 - y0);
    for (int y = y0; y <= y1; y++) {
      int x = x0 + slope * (y - y0);
      display_draw_pixel(x, y, color);
    }
  }
}

void display_draw_char(int *x, int y, char ch, const struct font_descriptor *f,
  int color)
{
  const struct font_glyph *g;
  const uint8_t *p;

  g = font_get_glyph(f, ch);
  if (!g)
    return;

  p = &f->data[g->bitmap_offset];
  for (int cy = 0; cy < g->height; ++cy) {
    for (int cx = 0; cx < g->width; ++cx) {
      int bit_no = cy * g->width + cx;
      int bit_pos = 7 - (bit_no % 8);
      uint8_t b = p[bit_no / 8];
      int val = (b >> bit_pos) & 1;
      if (!color)
        val = (~val) & 1;

      display_draw_pixel(*x + cx, y + g->height - cy, val);
    }
  }
  *x += g->x_advance;
}

bool display_get_char_size(char ch, const struct font_descriptor *f,
  int *width, int *height, int *x_advance)
{
  const struct font_glyph *g;

  if (!f || !width || !height || !x_advance)
    return false;

  g = font_get_glyph(f, ch);
  if (!g)
    return false;

  *width = g->width;
  *height = g->height;
  *x_advance = g->x_advance;
  return true;
}

int display_draw_text(int x, int y, const char *text,
  const struct font_descriptor *f, int color)
{
  const char *p = text;
  while(1) {
    char c = *p++;
    if (!c)
      break;
    display_draw_char(&x, y, c, f, color);
  }
  return x;
}

bool display_get_text_size(const char *text, const struct font_descriptor *f,
  int *size_x, int *size_y, bool with_spacing)
{
  int result_x = 0;
  int result_y = 0;
  int advance;
  int char_width;
  const char *p = text;

  if (!text || !f || !size_x || !size_y)
    return false;

  while(1) {
    int h;
    char c = *p++;
    if (!c)
      break;

    if (!display_get_char_size(c, f, &char_width, &h, &advance))
      return false;

    if (result_y < h)
      result_y = h;

    result_x += advance;
  }

  if (!with_spacing) {
    result_x -= advance;
    result_x += char_width;
  } else {
    result_y = f->y_advance;
  }

  *size_x = result_x;
  *size_y = result_y;
  return true;
}

int display_get_pixel(int x, int y)
{
  int page_idx = y / display_size_y;
  int bitpos = y % display_size_y;
  int byte_idx = page_idx * display_size_x + x;
  uint8_t b = display_buffer[byte_idx];
  return (b >> bitpos) & 1;
}

void display_init(uint8_t *buffer, int size_x, int size_y, int y_per_byte)
{
  display_buffer = buffer;
  display_size_x = size_x;
  display_size_y = size_y;
  display_y_per_byte = y_per_byte;
}

display_size_t display_get_size(void)
{
  return display_size_x | (display_size_y << 16);
}

void display_clear(void)
{
  memset(display_buffer, 0,
    display_size_y / display_y_per_byte * display_size_x);
}

