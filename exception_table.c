#include <stdint.h>
extern void reset_handler(void);
extern void tim2_handler(void);

typedef void (*exception_handler_fn)(void);

__attribute__((section(".exception_table")))

exception_handler_fn exception_table[0x130 / 4] = {
  [1] = ((uint32_t)reset_handler) + 1,
  [44] = ((uint32_t)tim2_handler) + 1
};
