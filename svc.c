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
    case SVC_PANIC:
      asm volatile("bkpt");
      break;
    default:
      break;
  }
}

