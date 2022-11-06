#include "config.h"
#include "systick.h"
#include "memory_layout.h"
#include "reg_access.h"

#define STK_CTRL (volatile uint32_t *)(SYSTICK_BASE + 0x00)
#define STK_CTRL_ENABLE 0
#define STK_CTRL_ENABLE_WIDTH 1
#define STK_CTRL_TICKINT 1
#define STK_CTRL_TICKINT_WIDTH 1
#define STK_CTRL_CLKSOURCE 2
#define STK_CTRL_CLKSOURCE_WIDTH 1
#define STK_CTRL_COUNTFLAG 16
#define STK_CTRL_COUNTFLAG_WIDTH 1

#define STK_LOAD (volatile uint32_t *)(SYSTICK_BASE + 0x04)
#define STK_VAL  (volatile uint32_t *)(SYSTICK_BASE + 0x08)
#define STK_CALIB (volatile uint32_t *)(SYSTICK_BASE + 0x0c)

extern void idle(void);

uint32_t wait_complete;

#define F_SYSTICK (F_CPU / 8)

void systick_set(uint32_t ms)
{
  uint32_t reload_value = F_SYSTICK / 1000 * ms;
  reg_write(STK_LOAD, reload_value);
  reg_write(STK_CTRL,
    (1<<STK_CTRL_ENABLE) |
    (1<<STK_CTRL_TICKINT)
  );
}

void systick_wait_ms(uint32_t ms)
{
  /*
   * if scheduler is enabled, systick is used by it in auto-repeat mode, so
   * this function should not be used
   */
  reg_write(STK_LOAD, (uint32_t)((float)0x44aa20 / 1000.0f) * ms);

  wait_complete = 0;
  reg_write(STK_CTRL,
    (1<<STK_CTRL_ENABLE) |
    (1<<STK_CTRL_TICKINT));

  idle();
}
