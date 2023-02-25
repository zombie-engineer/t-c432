#pragma once

void usb_init(void);

typedef enum {
  USB_CB_SET_ADDRESS,
  USB_CB_TX,
  USB_CB_RX,
} usb_driver_cb_t;

typedef void (*usb_driver_cb)(void *arg);

void usb_driver_set_cb(usb_driver_cb_t cb_type, usb_driver_cb cb);
