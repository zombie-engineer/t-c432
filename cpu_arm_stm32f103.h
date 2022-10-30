#pragma once
#include <stdint.h>

typedef uint32_t regtype;

/* A set of registers, put on the stack by stm32f103 upon exception entry */
struct hw_stored_ctx {
  regtype r0;
  regtype r1;
  regtype r2;
  regtype r3;
  regtype r12;
  regtype lr;
  regtype pc;
  regtype psr;
};

struct sw_stored_ctx {
  regtype r4;
  regtype r5;
  regtype r6;
  regtype r7;
  regtype r8;
  regtype r9;
  regtype r10;
  regtype r11;
  /* r12 is stored by HW */
  /* r13 is sp */
  /* r14 is lr, stored by HW */
  /* r15 is pc, stored by HW */
};
