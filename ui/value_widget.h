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

void value_widget_set_value(struct widget *w,
  const struct value_widget_value *v);

int value_widget_init(struct widget *w,
  const char *name_string,
  value_widget_type_t value_type,
  const struct value_widget_value *initial_value,
  const struct font_descriptor *f);
