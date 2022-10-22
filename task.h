#pragma once

typedef void (*task_entr_fn)(void *);

struct task {
  /* entry function */
  task_entr_fn entry_fn;
  /* task name */
  const char *name;
  /* pointer to context switching buffer */
  void *context;
};

int task_create(const char *task_name, task_entr_fn entry_fn);
