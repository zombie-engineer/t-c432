.arch armv7-m
.thumb
.globl reset_isr
.align 5
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
.align 3
memfault_isr:
1:
wfe
b 1b

.globl reserved_isr
.align 3
reserved_isr:
1:
wfe
b 1b

.globl svcall_isr
.align 3
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
bl save_context
ldr r0, [sp, #24]
sub r0, #2
ldrb r0, [r0]
bl svc_handler
b restore_context

.globl usgfault_isr
.align 3
usgfault_isr:
1:
b 1b

.macro load_current, dst_reg
  ldr \dst_reg, =current
  ldr \dst_reg, [\dst_reg]
.endm

.macro load_current_ctx, dst_reg
# context ptr is retrieved from struct task *current->ctx
# we first load address under the pointer of current, this is where the task is
# then in the task we read the field at offset 8 this is a struct task->ctx
# 
  ldr \dst_reg, =current
  ldr \dst_reg, [\dst_reg]
  ldr \dst_reg, [\dst_reg, #8]
.endm

.globl save_context
.align 4
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
# r0 - is the base of the context frame
# r0 moves to end of HW stacking
.macro do_save_context
mrs r0, psp
sub r0, #32
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

# Store top of the stacked process frame to current task->ctx
# First ,load address of 'current', then read value of current, this is the
# address of current task
ldr r1, =current
ldr r1, [r1]
# Next write r0 (top of processor frame on stack) to ctx field ot struct task
str r0, [r1, #8]
.endm
do_save_context
bx lr

.globl restore_context
.align 6
restore_context:
load_current_ctx r0
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
# Load PC with EXCEPTION_RETURN (thread mode, PSP stack)
ldr r0, =0xfffffffd
bx r0

.globl systick_isr
.align 5
systick_isr:
bl save_context
bl scheduler_select_next_current
b restore_context
