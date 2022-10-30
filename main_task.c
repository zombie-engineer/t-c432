#include "main_task.h"
#include "systick.h"

void main_task(void *)
{
  systick_set(100);
  while(1) {
  }
}
