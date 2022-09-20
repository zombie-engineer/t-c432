.thumb
.globl reset_isr
reset_isr:
mov r0, #0x20
lsl r0, r0, #24
mov r1, #0x10
lsl r1, r1, #8
add r0, r1
mov sp, r0
b main
1:
b 1b
