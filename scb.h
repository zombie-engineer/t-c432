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

bool scb_is_vector_table_hardfault(void);

int scb_is_interrupt_pending(void);

bool scb_is_in_nested_exception(void);

void scb_set_prigroup(int prigroup_value);

int scb_get_prigroup(void);

uint32_t scb_get_vector_table_address(void);

void scb_system_reset(void);

uint32_t scb_get_shcsr(void);

uint32_t scb_get_cfsr(void);

uint32_t scb_get_ccr(void);
uint32_t scb_thread_entry_ena(void);

uint32_t scb_get_icsr(void);

bool scb_memfault_is_access_violation(void);

typedef enum {
  USGFAULT_NONE,
  USGFAULT_TYPE_UNDEFINED_INSTR,
  USGFAULT_TYPE_INVALID_STATE,
  USGFAULT_TYPE_INVALID_PC_LOAD,
  USGFAULT_TYPE_NO_COPROC
} usgfault_type_t;

usgfault_type_t scb_get_usage_fault_type(void);
