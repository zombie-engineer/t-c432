#include "main_task.h"
#include "systick.h"
#include <usb_driver.h>
#include <config.h>

void main_task_fn(void *)
{
  systick_set(CNF_SCHEDULER_TICK_MS);
  timer_setup();
  usb_init();
  while(1) {
    asm volatile ("wfe");
  }
}
