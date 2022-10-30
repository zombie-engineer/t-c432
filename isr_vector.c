#include "compiler.h"
#include "cpu_arm_stm32f103.h"
#include "scb.h"

extern void reset_isr(void);
extern void nmi_isr(void);
extern void systick_isr(void);
extern void hardfault_isr(void);
extern void memfault_isr(void);
extern void bus_fault_isr(void);
extern void usgfault_isr(void);
extern void svcall_isr(void);
extern void tim2_isr(void);
extern void generic_isr(void);
extern void adc_isr(void);
extern void usb_hp_isr(void);
extern void usb_lp_isr(void);
extern void usb_wakeup_isr(void);
extern void reserved_isr(void);

__attribute__((section(".isr_vector")))

#define STACK_START 0x20006000
#define NUM_EXCEPTIONS (16 + 60)

#define ISR(__i, __fn, __is_thumb) \
  [__i] = ((int)__fn) + __is_thumb

int isr_vector[] = {
  ISR(0, STACK_START, 0),
  ISR(1, reset_isr, 1),
  ISR(2, nmi_isr, 1),
  ISR(3, hardfault_isr, 1),
  ISR(4, memfault_isr, 1),
  ISR(5, bus_fault_isr, 1),
  ISR(6, usgfault_isr, 1),
  ISR(7, reserved_isr, 1),
  ISR(8, reserved_isr, 1),
  ISR(9, reserved_isr, 1),
  ISR(10, reserved_isr, 1),
  ISR(11, svcall_isr, 1),
  ISR(12, generic_isr, 1),
  ISR(13, generic_isr, 1),
  ISR(14, generic_isr, 1),
  ISR(15, systick_isr, 1),
  ISR(16 + 18, adc_isr, 1),
  ISR(16 + 19, usb_hp_isr, 1),
  ISR(16 + 20, usb_lp_isr, 1),
  ISR(16 + 28, tim2_isr, 1),
  ISR(16 + 42, usb_wakeup_isr, 1),
};

void hardfault_isr(void)
{
  uint32_t v = scb_get_shcsr();
  bool is_vector_table_hardfault = scb_is_vector_table_hardfault();
  // BRK;
}

void bus_fault_isr(void) { }
void generic_isr(void) { }
void nmi_isr(void) {}

