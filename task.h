#pragma once
#include <stdint.h>
#include "list.h"

typedef void (*task_entr_fn)(void *);
typedef void (*task_exit_fn)(void);

#define STACK_SIZE 256

struct stack {
  uint32_t stack[64];
#if 0
  uint32_t top;
  uint32_t stack[STACK_SIZE / sizeof(uint32_t) - 2];
  uint32_t base;
#endif
};

struct task {
  struct list_node scheduler_list;
  /* task name */
  const char *name;
  /* pointer to context switching buffer */
  struct stack *stack;
};

struct task *task_create(const char *task_name, task_entr_fn entry_fn,
  task_exit_fn exit_fn);

uint32_t *task_get_process_frame(const struct task *t);
