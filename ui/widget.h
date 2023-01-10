#pragma once
#include <list.h>
#include "ui.h"

struct widget;

typedef void (*handle_event_fn)(struct widget *w, ui_event_type ev, int param);
typedef void (*draw_fn)(struct widget *w);
typedef void (*tick_fn)(struct widget *w, int tick_ms);

struct widget {
  struct list_node list;
  int pos_x;
  int pos_y;
  int size_x;
  int size_y;
  handle_event_fn handle_event;
  draw_fn draw;
  tick_fn on_tick;
  struct widget *parent;
  void *priv;
};
