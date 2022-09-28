.arch armv7-m
.thumb
.globl reset_isr
reset_isr:
mov r0, #0x20
lsl r0, r0, #24
mov r1, #0x10
lsl r1, r1, #8
add r0, r1
mov sp, r0

bl main
ldr r0, =0x40021018
ldr r1, [r0]
mov r2, #1
lsl r2, #2
orr r1, r2
str r1, [r0]
ldr r0, =0x4002101c
ldr r1, [r0]
mov r2, #1
lsl r2, #17
orr r1, r2
str r1, [r0]

# CRL PA2
ldr r0, =0x40010800
ldr r1, [r0]

# value |= 3 << 8
mov r2, #3
bfi r1, r2, #8, #2

# value |= 2 << 10
bfc r1, #10, #2
mov r2, #2
bfi r1, r2, #10, #2


# CRL PA3
# value ~= (3 << 12)
bfc r1, #12, #2

# value ~= (3 << 14)
bfc r1, #14, #2
# value |= 2 << 14
mov r2, #2
bfi r1, r2, #14, #2
ldr r0, =0x40010800
str r1, [r0]

ldr r0, =0x4001080c
ldr r1, [r0]
mov r2, #1
bfi r1, r2, #3, #1
str r1, [r0]

.globl usart_init2
usart_init2:
ldr r0, =0x4000440c
ldr r1, [r0]
# BIT 13 UE - Usart enable
mov r2, #1
bfi r1, r2, #13, #1
str r1, [r0]

mov r4, #0x45
ldr r3, =0x40004408
str r4, [r3]

# BIT 13 M = 0  Start bit, 8 data bits, n Stop bit
bfc r1, #12, #1
# BIT 3 TE: transmitter enabled
bfi r1, r2, #3, #1
# BIT 2 RE: receiver enabled
bfi r1, r2, #3, #1
str r1, [r0]
.globl usart_complete
usart_complete:





bl main
1:
b 1b


