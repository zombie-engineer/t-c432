#pragma once
#include "task.h"

void scheduler_init(void);

void scheduler_start(void);

void scheduler_enqueue_runnable(struct task *t);

void scheduler_exit_task(void);

void scheduler_select_next_current(void);

void scheduler_jump_to_main(void);
