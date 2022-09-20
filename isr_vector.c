extern void reset_isr(void);
extern void nmi_isr(void);
extern void hardfault_isr(void);
extern void mem_manage_isr(void);
extern void bus_fault_isr(void);
extern void usage_fault_isr(void);
extern void tim2_isr(void);
extern void generic_isr(void);

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
  ISR(4, mem_manage_isr, 1),
  ISR(5, bus_fault_isr, 1),
  ISR(6, usage_fault_isr, 1),
  ISR(7, generic_isr, 1),
  ISR(8, generic_isr, 1),
  ISR(9, generic_isr, 1),
  ISR(10, generic_isr, 1),
  ISR(11, generic_isr, 1),
  ISR(12, generic_isr, 1),
  ISR(13, generic_isr, 1),
  ISR(14, generic_isr, 1),
  ISR(15, generic_isr, 1),
  ISR(16 + 28, tim2_isr, 1),
};

void hardfault_isr(void) { }
void mem_manage_isr(void) { }
void bus_fault_isr(void) { }
void usage_fault_isr(void) { }
void generic_isr(void) { }
void nmi_isr(void) {}

