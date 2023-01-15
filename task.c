#include "task.h"
#include "scheduler.h"
#include "common_util.h"
#include "compiler.h"
#include "reg_access.h"
#include "cpu_arm_stm32f103.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
 * 8K of RAM 1/6 will be for stacks = 1280 bytes for stacks
 * Lets support at most 5 tasks, then 256 bytes for 1 stack
 */
#define MAX_TASKS 5
#define NUM_STACKS MAX_TASKS

struct regset_stm32f103 {
  regtype gp[16];
  regtype psr;
};

SECTION(".stack") struct stack stack_pool[NUM_STACKS];

static uint32_t stack_pool_busy_mask = 0;

static struct task task_pool[MAX_TASKS];

static uint32_t task_pool_busy_mask = 0;

static inline struct stack *stack_pool_get(void)
{
  int i;
  struct stack *result = NULL;
  int max_stack_bits = min(bitmask_size(stack_pool_busy_mask), NUM_STACKS);
  for (i = 0; i < max_stack_bits; ++i) {
    if (!u32_bit_is_set(stack_pool_busy_mask, i)) {
      u32_set_bit(&stack_pool_busy_mask, i);
      result = &stack_pool[i];
      for (int i = 0; i < STACK_SIZE; ++i) {
        result->raw[i] = 0xfeefdead;
      }
      break;
    }
  }
  return result;
}

static inline void stack_pool_put(struct stack *s)
{
  int bitmask_pos = s - stack_pool;
  u32_clear_bit(&stack_pool_busy_mask, bitmask_pos);
}

static inline struct task *task_pool_get(void)
{
  int i;
  int max_task_bits = min(bitmask_size(task_pool_busy_mask), MAX_TASKS);
  for (i = 0; i < max_task_bits; ++i) {
    if (!u32_bit_is_set(task_pool_busy_mask, i)) {
      u32_set_bit(&task_pool_busy_mask, i);
      return &task_pool[i];
    }
  }
  return NULL;
}

void task_init_process_frame(struct task *t,
  task_entr_fn entry_fn,
  task_exit_fn exit_fn)
{
  struct hw_stored_ctx *p;
  struct sw_stored_ctx *psw;
  p = &t->ctx->hw;
  p->r0  = 0x00000000;
  p->r1  = 0x01010101;
  p->r2  = 0x02020202;
  p->r3  = 0x03030303;
  p->r12 = 0x12121212;
  p->lr  = (uint32_t)exit_fn;
  p->pc  = (uint32_t)entry_fn;
  p->psr = 0x01000000;
  psw = &t->ctx->sw;

  psw->r4 = 0x04040404;
  psw->r5 = 0x05050505;
  psw->r6 = 0x06060606;
  psw->r7 = 0x07070707;
  psw->r8 = 0x08080808;
  psw->r9 = 0x09090909;
  psw->r10 = 0x10101010;
  psw->r11 = 0x11111111;
}

struct task *task_create(const char *task_name, task_entr_fn entry_fn,
  task_exit_fn exit_fn)
{
  struct task *t;
  struct stack *s;
  int init_ctx_pos;

  s = stack_pool_get();
  if (!s)
    return NULL;

  t = task_pool_get();
  if (!t) {
    stack_pool_put(s);
    return NULL;
  }

  t->name = task_name;
  t->stack = s;

  /*
   * Context is stored on the task's stack so t->ctx with store the address
   * somewhere on the stack to where processor + system  has stacked the
   * context at interrupt. Initially the context is on the stack bottom
   */
  init_ctx_pos = STACK_SIZE - sizeof(struct context) / sizeof(uint32_t);

  t->ctx = (struct context *)&s->raw[init_ctx_pos];

  task_init_process_frame(t, entry_fn, exit_fn);
  list_init(&t->scheduler_list);

  return t;
}

uint32_t *task_get_context_ptr(const struct task *t)
{
  return (uint32_t *)t->ctx;
}

void task_wait_ms(uint32_t ms)
{
  scheduler_task_wait_ms(ms);
}

uint32_t task_context_get_reg(const struct task *t, int reg_idx)
{
  struct hw_stored_ctx *p;
  p = &t->ctx->hw;
  return p->r0;
}
