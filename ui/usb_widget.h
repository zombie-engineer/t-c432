#pragma once
#include "widget.h"
#include <stdint.h>

int usb_widget_init(struct widget *w, nextprev_fn prev, nextprev_fn next);

void usb_widget_set_rx(const char *buf);

void usb_widget_set_tx(const char *buf);
