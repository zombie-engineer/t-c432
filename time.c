#include "time.h"
#include "scheduler.h"
#include "systick.h"

void wait_ms(uint32_t ms)
{
  scheduler_wait_ms(ms);
}
