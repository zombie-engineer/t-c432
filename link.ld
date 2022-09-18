MEMORY {
  RAM (rw): ORIGIN = 0x20000000, LENGTH = 8K
  FLASH (rx): ORIGIN = 0x08000000, LENGTH = 64K
}
SECTIONS {
  .exception_table : { exception_table.o(.exception_table) } > FLASH
  .text : { start.o(.text) main.o(.text) } > FLASH
}
