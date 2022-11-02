#include "main_task.h"
#include "systick.h"

void main_task(void *)
{
  systick_set(40);
  while(1) {
  }
}
