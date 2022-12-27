#include "main_task.h"
#include "systick.h"

void main_task(void *)
{
  systick_set(40);
  timer_setup();
  while(1) {
  }
}
