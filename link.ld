MEMORY {
  RAM (rw): ORIGIN = 0x20000000, LENGTH = 8K
  FLASH (rx): ORIGIN = 0x08000000, LENGTH = 64K
}
SECTIONS {
  .isr_vector : { isr_vector.o(.isr_vector) } > FLASH
  .text : { exti.o(.text) start.o(.text) main.o(.text) } > FLASH
  .main_stack(NOLOAD):
  {
    . = ALIGN(4);
    . += 0x400;
    __main_stack_base = .;
  } > RAM
  .rodata :
  {
    *.o(.rodata*)
  } > FLASH
  .bss :
  {
    . = ALIGN(4);
    __bss_start = .;
    *.o(.bss)
    . = ALIGN(4);
    __bss_end = .;
  } > RAM
  .stack(NOLOAD):
  {
  } > RAM
}
