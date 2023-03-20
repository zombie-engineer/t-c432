#pragma once
#include <stdint.h>
#include <stdbool.h>

struct font_descriptor;

typedef uint32_t display_size_t;

#define DISPLAY_SIZE_GET_X(__dsize) (__dsize & 0xffff)
#define DISPLAY_SIZE_GET_Y(__dsize) ((__dsize >> 16) & 0xffff)

display_size_t display_get_size(void);

void display_init(uint8_t *buffer, int size_x, int size_y, int y_per_byte);

void display_draw_pixel(int x, int y, int color);
void display_draw_filled_rect(int x0, int y0, int x1, int y1, int color);
void display_draw_hatched_rect(int x0, int y0, int x1, int y1, int color);
void display_draw_rect(int x0, int y0, int x1, int y1, int color);
void display_draw_line(int x0, int y0, int x1, int y1, int color);

void display_draw_char(int *x, int y, char ch, const struct font_descriptor *f,
  int color);

bool display_get_char_size(char ch, const struct font_descriptor *f,
  int *width, int *height, int *x_advance);

int display_draw_text(int x, int y, const char *text,
  const struct font_descriptor *f, int color);

bool display_get_text_size(const char *text, const struct font_descriptor *f,
  int *size_x, int *size_y, bool with_spacing);

int display_get_pixel(int x, int y);
void display_clear(void);
