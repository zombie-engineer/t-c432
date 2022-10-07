#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "reg_access.h"

struct scb_cpuid {
  char revision;
  char part_no;
  char constant;
  char variant;
  char implementer;
};

void scb_get_cpuid(struct scb_cpuid *i);

int scb_get_current_interrupt_no(void);

int scb_get_next_pending_interrupt_no(void);

int scb_is_interrupt_pending(void);

bool scb_is_in_nested_exception(void);

uint32_t scb_get_vector_table_address(void);

void scb_system_reset(void);
