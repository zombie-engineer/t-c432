#pragma once
#include "task.h"

struct scheduler_stats {
  int task_switches;
};

extern struct scheduler_stats sched_stats;

void scheduler_init(void);

void scheduler_start(void);

void scheduler_enqueue_runnable(struct task *t);

void scheduler_exit_task(void);

void scheduler_select_next_current(void);

void scheduler_jump_to_main(void);
