.thumb
.globl reset_handler
reset_handler:
# stack at 0x20001000
mov r0, #0x20
lsl r0, r0, #24
mov r1, #0x10
lsl r1, r1, #8
add r0, r1
mov sp, r0


# mov sp, r0
# add sp, #0x1000
b main
1:
b 1b
.globl _start
_start:
