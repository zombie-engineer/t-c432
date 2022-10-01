#include "flash.h"
#include "reg_access.h"
#include "memory_layout.h"
#include <stdint.h>

#define FLASH_ACR (volatile uint32_t *)(FLASH_BASE + 0x00)
/* Latency selector based on SYSCLK speed */
#define FLASH_ACR_LATENCY 0
#define FLASH_ACR_LATENCY_WIDTH 2
#define FLASH_ACR_LATENCY_0_24_MHZ 0
#define FLASH_ACR_LATENCY_24_48_MHZ 1
#define FLASH_ACR_LATENCY_48_72_MHZ 2

/* Half cycle enabled */
#define FLASH_ACR_HLFCYA 3
#define FLASH_ACR_HLFCYA_WIDTH 1
/* Prefetch buffer enable */
#define FLASH_ACR_PRFTBE 4
#define FLASH_ACR_PRFTBE_WIDTH 1

/* Prefetch buffer status */
#define FLASH_ACR_PRFTBS 5
#define FLASH_ACR_PRFTBS_WIDTH 1

void flash_enable_prefetch(void)
{
  reg_write(FLASH_ACR,
    (FLASH_ACR_LATENCY_48_72_MHZ << FLASH_ACR_LATENCY) | (1 << FLASH_ACR_PRFTBE));
}
