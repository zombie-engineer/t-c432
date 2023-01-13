#pragma once
#include "widget.h"

struct font_descriptor;

int text_widget_init(struct widget *w, const char *text,
  const struct font_descriptor *f);

void text_widget_activate(struct widget *w);
