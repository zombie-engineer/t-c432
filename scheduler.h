#pragma once
#include "task.h"

struct scheduler_stats {
  int task_switches;
  int task_sleeps;
  int num_runnable_tasks;
};

extern struct scheduler_stats sched_stats;

void scheduler_init(void);

void scheduler_start(struct task *main_task);

void scheduler_enqueue_runnable(struct task *t);

void scheduler_exit_task(void);

void scheduler_jump_to_main(void);

void scheduler_task_sleep();

void scheduler_wait_on_flag();
void scheduler_signal_flag_irq(uint32_t *flag);
