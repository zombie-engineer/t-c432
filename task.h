#pragma once
#include <stdint.h>
#include "list.h"
#include "cpu_arm_stm32f103.h"

typedef void (*task_entr_fn)(void *);
typedef void (*task_exit_fn)(void);

#define STACK_SIZE 128

struct context {
  struct sw_stored_ctx sw;
  struct hw_stored_ctx hw;
};

struct stack {
  uint32_t raw[STACK_SIZE];
};

struct task {
  struct list_node scheduler_list;
  struct context *ctx;
  /* task name */
  const char *name;
  /* pointer to context switching buffer */
  struct stack *stack;

  int timer;
  uint32_t *wait_on_flag;
};

struct task *task_create(const char *task_name, task_entr_fn entry_fn,
  task_exit_fn exit_fn);

uint32_t *task_get_context_ptr(const struct task *t);

void task_init_process_frame(struct task *t,
  task_entr_fn entry_fn,
  task_exit_fn exit_fn);

uint32_t task_context_get_reg(const struct task *t, int reg_idx);
