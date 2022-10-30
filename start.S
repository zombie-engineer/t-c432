.arch armv7-m
.thumb
.globl reset_isr
reset_isr:
# SCB_SHCSR Enable MEMFAULT, BUSFAULT, USGFAULT handling
ldr r0, =0xe000ed24
ldr r2, [r0]
mov r1, #7
bfi r2, r1, #16, #3
str r2, [r0]



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


.globl idle
idle:
1:
ldr r0, =wait_complete
ldr r0, [r0]
cmp r0, #1
beq 2f
wfi
b 1b
2:
bx lr

.globl memfault_isr
memfault_isr:
1:
wfe
b 1b

.globl reserved_isr
reserved_isr:
1:
wfe
b 1b

.globl svcall_isr
svcall_isr:
# What is the argeument to SVC?
# SVC syntax is 'svc imm', where 'imm' is numeric svc argument
# it is not passed to interrupt handler. The software should extract 'imm'
# itself by looking on the stacked pc address and extracting 'imm' from the
# bytecode
# So stacked pc is located this way:
# On stack top-to-bottom: r0, r1, r2, r3, r12, lr, pc
#         offset from sp  +0  +4  +8 +12  +16 +20 +24
# Means sp is pointing on r0, and pc is stacked at sp + 24
# After pc is retreived we need to look at the svc imm instruction, it is
# 2 bytes long, and pc is looking at the next instruction after it so to
# read svc imm instruction we need to read 2 bytes which are 2 bytes before the
# stacked pc.
# svc encoding is (SVC opcode << 8) | (imm & 0xf)
# imm byte goes first, so we can just read 1 byte at offset pc - 2
ldr r0, [sp, #24]
sub r0, #2
ldrb r0, [r0]
bl svc_handler
b restore_context

.globl usgfault_isr
usgfault_isr:
1:
b 1b

.macro load_current, dst_reg
  ldr \dst_reg, =current
  ldr \dst_reg, [\dst_reg]
.endm

.macro load_current_ctx, dst_reg
  ldr \dst_reg, =current_ctx
  ldr \dst_reg, [\dst_reg]
.endm

save_context:
# first column is old_sp offset
# second column is new_sp offset

# -60 +0  r4 <- SW stacking ends here <- current_ctx points here
# -56 +4  r5
# -52 +8  r6
# -48 +12 r7
# -44 +16 r8    <- [r0, #0]
# -40 +20 r9    <- [r0, #4]
# -36 +24 r10   <- [r0, #8]
# -32 +28 r11   <- [r0, #12] SW stacking start here ^direction
# -28 +32 r0 <- HW stacking ends here
# -24 +36 r1
# -20 +40 r2
# -16 +44 r3
# -12 +48 r12
# -8  +52 lr
# -4  +56 pc
# -0  +60 psr <- HW stacking starts here
bkpt
load_current_ctx r0
# r0 - is the base of the context frame
cbz r0, 1f
# r0 moves to end of HW stacking
sub r0, #28
sub r0, #36
str r4, [r0, #0]
str r5, [r0, #4]
str r6, [r0, #8]
str r7, [r0, #12]
mov r1, r8
str r1, [r0, #16]
mov r1, r9
str r1, [r0, #20]
mov r1, r10
str r1, [r0, #24]
mov r1, r11
str r1, [r0, #28]
1:
bx lr

restore_context:
load_current_ctx r0
cbz r0, 1f
ldm r0!, {r4-r7}
ldr r1, [r0, #0]
mov r8, r1
ldr r1, [r0, #4]
mov r9, r1
ldr r1, [r0, #8]
mov r10, r1
ldr r1, [r0, #12]
mov r11, r1
add r0, #16
msr psp, r0
isb
1:
ldr r0, =0xfffffffd
push {r0}
pop {pc}

.globl systick_isr
systick_isr:
bl save_context
bl scheduler_select_next_current
b restore_context
