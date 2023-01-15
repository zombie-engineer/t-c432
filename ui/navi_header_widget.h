#pragma once
#include "widget.h"

struct font_descriptor;

int navi_header_widget_init(struct widget *w,
  int x,
  int y,
  int sx,
  int sy,
  nextprev_fn prev,
  nextprev_fn next,
  const char *name,
  const struct font_descriptor *font
  );
