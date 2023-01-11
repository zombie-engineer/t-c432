#include "main_task.h"
#include "systick.h"
#include <config.h>

void main_task_fn(void *)
{
  systick_set(CNF_SCHEDULER_TICK_MS);
  timer_setup();
  while(1) {
    asm volatile ("wfe");
  }
}
