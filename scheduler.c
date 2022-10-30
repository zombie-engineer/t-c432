#include "scheduler.h"
#include "task.h"
#include "common_util.h"
#include "systick.h"
#include "svc.h"
#include "main_task.h"

static struct list_node runnable = { 0 };
struct task *current = 0;

void scheduler_select_next_current(void)
{
  struct task *t;
  struct list_node *list_task;
  list_task = list_remove_head(&runnable);
  if (!list_task) {
    while(1);
  }

  current = container_of(list_task, struct task, scheduler_list);
  list_add_tail(&runnable, list_task);
}

void scheduler_start(void)
{
  struct task *main_task = NULL;
  struct list_node *l;
  struct context t;
  l = list_get_first(&runnable);
  main_task = container_of(l, struct task, scheduler_list);
  current = main_task;
  main_task->ctx = &t;
  /* */
  asm volatile (
    "mov r0, %0\n"
    "msr psp, r0\n"
    :: "r"(&main_task->stack->raw[STACK_SIZE]));
  svc_call(SVC_JUMP_TO_MAIN);
  /* should not get here */
}

void scheduler_enqueue_runnable(struct task *t)
{
  list_add_tail(&runnable, &t->scheduler_list);
}

void scheduler_init(void)
{
  list_init(&runnable);
}

void scheduler_exit_task(void)
{
}

void scheduler_jump_to_main(void)
{
  int init_ctx_pos = STACK_SIZE - sizeof(struct context) / sizeof(uint32_t);
  current->ctx = (struct context *)&current->stack->raw[init_ctx_pos];
  task_init_process_frame(current, main_task, scheduler_exit_task);
}
