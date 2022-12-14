#include "font.h"
#include <stddef.h>
#include "fonts/font_dejavu_sans_mono_8.h"
#include "fonts/font_dejavu_sans_mono_9.h"
#include "fonts/font_dejavu_sans_mono_10.h"
#include "fonts/font_dejavu_sans_mono_12.h"


const struct font_glyph *font_get_glyph(const struct font_descriptor *f, char ch)
{
  if (ch < f->start_char || ch >= f->last_char)
    return NULL;

  return &f->glyphs[ch - f->start_char];
}

int font_get_string_width(const struct font_descriptor *f, const char *str)
{
  int result = 0;
  const struct font_glyph *g;
  char ch;

  while(1) {
    ch = *str++;
    if (!ch)
      break;
    g = font_get_glyph(f, ch);
    if (ch) {
      result += g->x_advance;
    }
  }
  return result;
}
