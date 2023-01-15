#pragma once
#include <list.h>
#include "ui.h"

struct widget;

typedef void (*nextprev_fn)(void);
typedef void (*handle_event_fn)(struct widget *w, ui_event_type ev, int param);
typedef void (*draw_fn)(struct widget *w);

struct widget {
  struct list_node list;
  int pos_x;
  int pos_y;
  int size_x;
  int size_y;
  handle_event_fn handle_event;
  draw_fn draw;
  struct widget *parent;
  void *priv;
};
