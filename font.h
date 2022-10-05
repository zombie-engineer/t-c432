#pragma once
#include <stdint.h>

struct font_glyph {
  uint32_t bitmap_offset;
  int width;
  int height;
  int x_advance;
  int x_offset;
  int y_offset;
};

struct font_descriptor {
  const struct font_glyph *glyphs;
  const uint8_t *data;
  int start_char;
  int last_char;
  int y_advance;
};

extern const struct font_descriptor font_1;
extern const struct font_descriptor font_2;
extern const struct font_descriptor font_3;
extern const struct font_descriptor font_4;

const struct font_glyph *font_get_glyph(const struct font_descriptor *f, char ch);
