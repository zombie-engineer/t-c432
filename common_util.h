#pragma once
#include <stddef.h>

#define min(__a, __b) ((__a) < (__b) ? (__a) : (__b))

#define ARRAY_SIZE(__a) (sizeof(__a) / sizeof(__a[0]))

/*
 * linux-kernel style macro, arguments are the same on purpose to not
 * confuse developers offsetof is supported by gcc
 */
#define container_of(__ptr, __type, __member) \
  (__type *)(((char *)(__ptr)) - offsetof(__type, __member))
