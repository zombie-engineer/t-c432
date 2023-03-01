#pragma once
#include "widget.h"

int bitmap_widget_init(struct widget *w,
  int x,
  int y,
  int sx,
  int sy,
  const void *bitmap);

void text_widget_set_bitmap(struct widget *w, const void *data);
