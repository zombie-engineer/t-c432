#pragma once
#include "widget.h"
#include <stdint.h>

struct font_descriptor;

typedef enum {
  VALUE_TYPE_INT,
  VALUE_TYPE_FLOAT,
  VALUE_TYPE_STRING,
} value_widget_type_t;

struct value_widget_value {
  union {
  uint32_t int_value;
  float float_value;
  } u;
};

int value_widget_init(struct widget *w,
  value_widget_type_t value_type,
  const struct value_widget_value *initial_value,
  const struct font_descriptor *f);
