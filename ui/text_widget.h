#pragma once
#include "widget.h"

struct font_descriptor;

void text_widget_activate(struct widget *w);

int text_widget_init(struct widget *w,
  int x,
  int y,
  int sx,
  int sy,
  const char *text,
  const struct font_descriptor *font);
