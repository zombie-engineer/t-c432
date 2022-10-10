#include "scb.h"

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

#define SCB_ICSR (volatile uint32_t *)(SCB_BASE + 0x04)
#define SCB_ICSR_VECTACTIVE 0
#define SCB_ICSR_VECTACTIVE_WIDTH 9
#define SCB_ICSR_RETOBASE 11
#define SCB_ICSR_RETOBASE_WIDTH 1
#define SCB_ICSR_VECTPENDING 12
#define SCB_ICSR_VECTPENDING_WIDTH 9
#define SCB_ICSR_ISRPENDING 22
#define SCB_ICSR_ISRPENDING_WIDTH 1
#define SCB_ICSR_PENDSTCLR 25
#define SCB_ICSR_PENDSTCLR_WIDTH 1
#define SCB_ICSR_PENDSTSET 26
#define SCB_ICSR_PENDSTSET_WIDTH 1
#define SCB_ICSR_PENDSVCLR 27
#define SCB_ICSR_PENDSVCLR_WIDTH 1
#define SCB_ICSR_PENDSVSET 28
#define SCB_ICSR_PENDSVSET_WIDTH 1
#define SCB_ICSR_NMIPENDSET 31
#define SCB_ICSR_NMIPENDSET_WIDTH 1

#define SCB_VTOR (volatile uint32_t *)(SCB_BASE + 0x08)
#define SCB_VTOR_TBLOFF 9
#define SCB_VTOR_TBLOFF_WIDTH 21
/* This one is a part of previous but can indicate location in memory */
#define SCB_VTOR_TBLBASE 29
#define SCB_VTOR_TBLBASE_WIDTH 1
#define SCB_VTOR_TBLBASE_CODE 0
#define SCB_VTOR_TBLBASE_SRAM 1

#define SCB_AIRCR (volatile uint32_t *)(SCB_BASE + 0x0c)
#define SCB_AIRCR_VECTRESET 0
#define SCB_AIRCR_VECTRESET_WIDTH 1
#define SCB_AIRCR_VECTCLRACTIVE 1
#define SCB_AIRCR_VECTCLRACTIVE_WIDTH 1
#define SCB_AIRCR_SYSRESETREQ 2
#define SCB_AIRCR_SYSRESETREQ_WIDTH 1
#define SCB_AIRCR_PRIGROUP 8
#define SCB_AIRCR_PRIGROUP_WIDTH 3
#define SCB_AIRCR_ENDIANESS 15
#define SCB_AIRCR_ENDIANESS_WIDTH 1
#define SCB_AIRCR_VECTKEYSTAT 16
#define SCB_AIRCR_VECTKEYSTAT_WIDTH 16
#define SCB_AIRCR_VECTKEY_READ_VALUE 0xfa05
#define SCB_AIRCR_VECTKEY_WRITE_VALUE 0x05fa

#define SCB_SHPR1 (volatile uint32_t *)(SCB_BASE + 0x18)

void scb_get_cpuid(struct scb_cpuid *i)
{
  uint32_t v = reg_read(SCB_CPUID);
  i->revision = u32_extract_bits(v, SCB_CPUID_REVISION,
    SCB_CPUID_REVISION_WIDTH);
  i->part_no = u32_extract_bits(v, SCB_CPUID_PARTNO,
    SCB_CPUID_PARTNO_WIDTH);
  i->constant = u32_extract_bits(v, SCB_CPUID_CONSTANT,
    SCB_CPUID_CONSTANT_WIDTH);
  i->variant = u32_extract_bits(v, SCB_CPUID_VARIANT,
    SCB_CPUID_VARIANT_WIDTH);
  i->implementer = u32_extract_bits(v, SCB_CPUID_IMPLEMENTER,
    SCB_CPUID_IMPLEMENTER_WIDTH);
}

int scb_get_current_interrupt_no(void)
{
  return u32_extract_bits(reg_read(SCB_ICSR),
    SCB_ICSR_VECTACTIVE, SCB_ICSR_VECTACTIVE_WIDTH);
}

int scb_get_next_pending_interrupt_no(void)
{
  return u32_extract_bits(reg_read(SCB_ICSR),
    SCB_ICSR_VECTPENDING, SCB_ICSR_VECTPENDING_WIDTH);
}

int scb_is_interrupt_pending(void)
{
  return reg32_bit_is_set(SCB_ICSR, SCB_ICSR_RETOBASE);
}

bool scb_is_in_nested_exception(void)
{
  return reg32_bit_is_set(SCB_ICSR, SCB_ICSR_ISRPENDING);
}

uint32_t scb_get_vector_table_address(void)
{
  return u32_extract_bits(reg_read(SCB_VTOR),
    SCB_VTOR_TBLOFF, SCB_VTOR_TBLOFF_WIDTH) << SCB_VTOR_TBLOFF;
}

void scb_set_prigroup(int prigroup_value)
{
  uint32_t v = reg_read(SCB_AIRCR);

  u32_modify_bits(&v, SCB_AIRCR_VECTKEYSTAT, SCB_AIRCR_VECTKEYSTAT_WIDTH,
    SCB_AIRCR_VECTKEY_WRITE_VALUE);

  u32_modify_bits(&v, SCB_AIRCR_PRIGROUP, SCB_AIRCR_PRIGROUP_WIDTH,
    prigroup_value);

  reg_write(SCB_AIRCR, v);
}

int scb_get_prigroup(void)
{
  return u32_extract_bits(reg_read(SCB_AIRCR),
    SCB_AIRCR_PRIGROUP, SCB_AIRCR_PRIGROUP_WIDTH);
}

void scb_system_reset(void)
{
  uint32_t v = reg_read(SCB_AIRCR);

  u32_modify_bits(&v, SCB_AIRCR_VECTKEYSTAT, SCB_AIRCR_VECTKEYSTAT_WIDTH,
    SCB_AIRCR_VECTKEY_WRITE_VALUE);
  u32_set_bit(&v, SCB_AIRCR_SYSRESETREQ);

  reg_write(SCB_AIRCR, v);
}