#pragma once
#include "widget.h"

typedef enum {
  ARROW_BUTTON_TYPE_LEFT,
  ARROW_BUTTON_TYPE_RIGHT,
} arrow_button_type_t;

int arrow_button_widget_init(struct widget *w,
  int x,
  int y,
  int sx,
  int sy,
  arrow_button_type_t type);

void arrow_button_widget_activate(struct widget *w);
