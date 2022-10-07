#include "scb.h"
#include <stdint.h>

#define SCB_BASE 0xe000ed00
#define SCB_CPUID (volatile uint32_t *)(SCB_BASE + 0x00)
#define SCB_CPUID_REVISION 0
#define SCB_CPUID_REVISION_WIDTH 4
#define SCB_CPUID_PARTNO 4
#define SCB_CPUID_PARTNO_WIDTH 12
#define SCB_CPUID_CONSTANT 16
#define SCB_CPUID_CONSTANT_WIDTH 4
#define SCB_CPUID_VARIANT 20
#define SCB_CPUID_VARIANT_WIDTH 4
#define SCB_CPUID_IMPLEMENTER 24
#define SCB_CPUID_IMPLEMENTER_WIDTH 6

void scb_get_cpuid(struct scb_cpuid *i)
{
  uint32_t v = reg_read(SCB_CPUID);
  i->revision = u32_extract_bits(v, SCB_CPUID_REVISION, SCB_CPUID_REVISION_WIDTH);
  i->part_no = u32_extract_bits(v, SCB_CPUID_PARTNO, SCB_CPUID_PARTNO_WIDTH);
  i->constant = u32_extract_bits(v, SCB_CPUID_CONSTANT, SCB_CPUID_CONSTANT_WIDTH);
  i->variant = u32_extract_bits(v, SCB_CPUID_VARIANT, SCB_CPUID_VARIANT_WIDTH);
  i->implementer = u32_extract_bits(v, SCB_CPUID_IMPLEMENTER, SCB_CPUID_IMPLEMENTER_WIDTH);
}
