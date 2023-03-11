#pragma once
#include "widget.h"
#include <stdint.h>

int led_widget_init(struct widget *w, nextprev_fn prev, nextprev_fn next);
