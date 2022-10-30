#include "svc.h"
#include "scheduler.h"

int svc_handler(char arg)
{
  switch(arg)
  {
    case SVC_JUMP_TO_MAIN:
      scheduler_jump_to_main();
      break;
    default:
      break;
  }
}

