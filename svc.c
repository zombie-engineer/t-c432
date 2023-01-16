#include "svc.h"
#include "scheduler.h"

int svc_handler(char arg)
{
  switch(arg)
  {
    case SVC_JUMP_TO_MAIN:
      scheduler_jump_to_main();
      break;
    case SVC_SLEEP:
      scheduler_task_sleep();
      break;
    case SVC_WAIT_ON_FLAG:
      scheduler_wait_on_flag();
      break;
    case SVC_PANIC:
      asm volatile("bkpt");
      break;
    default:
      break;
  }
}

int svc_wait_on_flag(uint32_t *flag)
{
  svc_call(SVC_WAIT_ON_FLAG);
}

void svc_wait_ms(uint32_t ms)
{
  svc_call(SVC_SLEEP);
}
