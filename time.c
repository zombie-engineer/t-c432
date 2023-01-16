#include "time.h"
#include "scheduler.h"
#include "systick.h"

void wait_ms(uint32_t ms)
{
  for (volatile int i = 0; i < 1000 * ms; ++i);
}

