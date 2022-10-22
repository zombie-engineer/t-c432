#include "task.h"
#include "common_util.h"
#include "reg_access.h"
#include <stdint.h>

/*
 * 8K of RAM 1/6 will be for stacks = 1280 bytes for stacks
 * Lets support at most 5 tasks, then 256 bytes for 1 stack
 */
#define STACK_SIZE 256
#define NUM_STACKS 5

struct stack {
  uint32_t stack_word[STACK_SIZE / sizeof(uint32_t)];
};

static struct stack stack_pool[NUM_STACKS];
static uint32_t stack_pool_busy_mask = 0;
#define bitmask_size(__bitmask) (sizeof(__bitmask) * 8)

/* Returns index in a stack pool or -1 if failed */
static inline int stack_pool_get(void)
{
  for (int i = 0; i < min(bitmask_size(stack_pool_busy_mask), NUM_STACKS); ++i) {
    if (!u32_bit_is_set(stack_pool_busy_mask, i)) {
      u32_set_bit(&stack_pool_busy_mask, i);
      return i;
    }
  }
  return -1;
}

int task_create(const char *task_name, task_entr_fn entry_fn)
{
  int i;
  i = stack_pool_get();
  if (i == -1)
    return -1;


  return 0;
}
