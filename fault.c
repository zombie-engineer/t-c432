#include <scb.h>

void __usgfault_isr(void)
{
  switch (scb_get_usage_fault_type())
  {
    case USGFAULT_NONE:
      break;
    case USGFAULT_TYPE_UNDEFINED_INSTR:
      asm volatile ("bkpt");
      break;
    case USGFAULT_TYPE_INVALID_STATE:
      asm volatile ("bkpt");
      break;
    case USGFAULT_TYPE_INVALID_PC_LOAD:
      asm volatile ("bkpt");
      break;
    case USGFAULT_TYPE_NO_COPROC:
      asm volatile ("bkpt");
      break;
    default:
      break;
  }
}
