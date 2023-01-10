#pragma once
#include "widget.h"

typedef enum {
  ARROW_BUTTON_TYPE_LEFT,
  ARROW_BUTTON_TYPE_RIGHT,
} arrow_button_type_t;

int arrow_button_widget_init(struct widget *w, arrow_button_type_t type);
