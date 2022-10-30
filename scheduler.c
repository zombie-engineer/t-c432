#include "scheduler.h"
#include "task.h"
#include "common_util.h"
#include "systick.h"
#include "svc.h"

static struct list_node runnable = { 0 };
struct task *current = 0;
uint32_t *current_ctx = 0;

void scheduler_select_next_current(void)
{
  struct task *t;
  struct list_node *list_task;
  list_task = list_remove_head(&runnable);
  if (!list_task) {
    while(1);
  }

  current = container_of(list_task, struct task, scheduler_list);
  current_ctx = task_get_process_frame(current);
  list_add_tail(&runnable, list_task);
}

void scheduler_start(void)
{
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
  scheduler_select_next_current();
}
