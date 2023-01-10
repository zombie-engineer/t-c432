#include "scheduler.h"
#include <config.h>
#include "task.h"
#include "common_util.h"
#include "systick.h"
#include "svc.h"
#include "scb.h"
#include "main_task.h"

static struct list_node runnable = { 0 };
static struct list_node time_waiting = { 0 };
struct task *current = 0;
struct task *idle_task;

struct scheduler_stats sched_stats = {
  .task_switches = 0
};

typedef enum {
  PREV_TASK_TO_TAIL = 0,
  PREV_TASK_TO_SLEEP = 1,
  PREV_TASK_TO_WAIT = 2,
} task_schedule_action_t;

static void scheduler_select_next_current(
  task_schedule_action_t prev_task_action,
  uint32_t sleep_ms)
{
  struct task *prev_current = current;
  struct task *t;

  struct list_node *l;

  if (list_is_empty(&runnable)) {
    if (!current)
    {
      list_add_tail(&runnable, &idle_task->scheduler_list);
    }
    else
    {
      return;
    }
  }

  /* Take next task from runnable list and assign it to 'current' */
  l = list_remove_head(&runnable);
  if (!l) {
    while(1);
  }

  /* Current is selected to run after exiting interrupt state */
  current = container_of(l, struct task, scheduler_list);
  sched_stats.task_switches++;

  /* Previous 'current' has to be put to the tail of runnable or elsewhere */
  if (prev_task_action == PREV_TASK_TO_TAIL)
  {
    list_add_tail(&runnable,  &prev_current->scheduler_list);
  }
  else if (prev_task_action == PREV_TASK_TO_SLEEP)
  {
    prev_current->timer = sleep_ms / CNF_SCHEDULER_TICK_MS;
    list_add_tail(&time_waiting, &prev_current->scheduler_list);
  }
}

void scheduler_check_time_waiting_tasks(void)
{
  struct task *t;
  struct list_node *l;
  struct list_node *l_tmp;
  l = time_waiting.next;
  while(l != &time_waiting) {
    l_tmp = l->next;
    t = container_of(l, struct task, scheduler_list);

    if (t->timer) {
      t->timer--;
    }
    if (!t->timer) {
      list_del(&t->scheduler_list);
      list_add_tail(&runnable, &t->scheduler_list);
    }
    l = l_tmp;
  }
}

void scheduler_tick(void)
{
  scheduler_check_time_waiting_tasks();
  scheduler_select_next_current(PREV_TASK_TO_TAIL, 0);
}

void scheduler_start(struct task *main_task)
{
  current = main_task;
  asm volatile (
    "mov r0, %0\n"
    "msr psp, r0\n"
    "mrs r0, control\n"
    "orr r0, r0, #2\n"
    "msr control, r0\n"
    :: "r"(&main_task->stack->raw[STACK_SIZE]));

  uint32_t icsr = scb_get_icsr();
  svc_call(SVC_JUMP_TO_MAIN);
  /* should not get here */
}

void scheduler_enqueue_runnable(struct task *t)
{
  list_add_tail(&runnable, &t->scheduler_list);
}

static void idle_fn(void *arg)
{
  while(1) {
    asm volatile ("wfe");
  }
}

void scheduler_init(void)
{
  list_init(&runnable);
  list_init(&time_waiting);
  idle_task = task_create("idle", idle_fn, scheduler_exit_task);
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

void scheduler_task_wait_ms(uint32_t ms)
{
  svc_call(SVC_SLEEP);
}

void scheduler_wait_ms(uint32_t ms)
{
  for (volatile int i = 0; i < 1000 * ms; ++i);
}

void scheduler_task_sleep()
{
  uint32_t ms = task_context_get_reg(current, 0);
  scheduler_select_next_current(PREV_TASK_TO_SLEEP, ms);
}
