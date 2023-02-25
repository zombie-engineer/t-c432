#include "main_task.h"
#include "systick.h"
#include <ui.h>
#include <usb_driver.h>
#include <config.h>

void usb_set_address_callback(void *arg)
{
  ui_on_set_address();
}

void usb_tx_callback(void *arg)
{
  ui_on_tx(arg);
}

void usb_rx_callback(void *arg)
{
  ui_on_rx(arg);
}

void main_task_fn(void *)
{
  systick_set(CNF_SCHEDULER_TICK_MS);
  timer_setup();
  usb_driver_set_cb(USB_CB_SET_ADDRESS, usb_set_address_callback);
  usb_driver_set_cb(USB_CB_RX, usb_rx_callback);
  usb_driver_set_cb(USB_CB_TX, usb_tx_callback);
  usb_init();
  while(1) {
    asm volatile ("wfe");
  }
}
