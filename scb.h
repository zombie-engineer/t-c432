#pragma once
#include "reg_access.h"

struct scb_cpuid {
  char revision;
  char part_no;
  char constant;
  char variant;
  char implementer;
};

void scb_get_cpuid(struct scb_cpuid *i);
