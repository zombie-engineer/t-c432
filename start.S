.arch armv7-m
.thumb
.globl reset_isr
.align 5
.equ SCB_SHCSR, 0xe000ed24

reset_isr:
# SCB_SHCSR Enable MEMFAULT, BUSFAULT, USGFAULT handling
ldr r0, =SCB_SHCSR
ldr r2, [r0]
mov r1, #7
bfi r2, r1, #16, #3
str r2, [r0]

ldr r0, =__main_stack_base
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

.ltorg

.macro ISR_START name
.globl \name\()_isr
.align 5
\name\()_isr:
bl save_context
.endm

.macro ISR_END
b restore_context
.endm

ISR_START svcall
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
ldr r0, =current
ldr r0, [r0]
ldr r0, [r0, #8]
ldr r0, [r0, #56]
sub r0, #2
ldrb r0, [r0]
bl svc_handler
ISR_END
.ltorg

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

ISR_START exti_0
mov r0, #0
bl exti_isr
ISR_END

ISR_START exti_1
mov r0, #1
bl exti_isr
ISR_END

ISR_START exti_2
mov r0, #2
bl exti_isr
ISR_END

ISR_START exti_3
mov r0, #3
bl exti_isr
ISR_END

ISR_START exti_4
mov r0, #4
bl exti_isr
ISR_END

ISR_START exti_9_5
ldr r0, =0x40010414
ldr r1, [r0]
mov r0, #5
lsr r1, #5
mov r2, #1
b 2f
1:
add r0, #1
lsr r1, #1
2:
tst r1, r2
beq 1b
bl exti_isr
ISR_END

ISR_START exti_15_10
ldr r0, =0x40010414
ldr r1, [r0]
mov r0, #10
lsr r1, #10
mov r2, #1
b 2f
1:
add r0, #1
lsr r1, #1
2:
tst r1, r2
beq 1b
bl exti_isr
ISR_END

.globl memfault_isr
.align 3
memfault_isr:
1:
bkpt
wfe
b 1b

.globl reserved_isr
.align 3
reserved_isr:
1:
bkpt
wfe
b 1b

ISR_START systick
bl scheduler_tick
ISR_END


ISR_START i2c1_ev
mov r0, #0
bl i2c1_isr
ISR_END

ISR_START i2c1_er
mov r0, #1
bl i2c1_isr
ISR_END

ISR_START tim2
bl __tim2_isr
ISR_END

ISR_START usb_hp
bl __usb_hp_isr
ISR_END

ISR_START usb_lp
bl __usb_lp_isr
ISR_END

ISR_START usb_wakeup
bl __usb_wakeup_isr
ISR_END

ISR_START usgfault
bl __usgfault_isr
ISR_END

.macro DMA_ISR dma_id, channel_id
ISR_START dma\()\dma_id\()_ch\()\channel_id
mov r0, #(\dma_id - 1)
mov r1, #(\channel_id - 1)
bl dma_isr
ISR_END
.endm

DMA_ISR 1, 1
DMA_ISR 1, 2
DMA_ISR 1, 3
DMA_ISR 1, 4
DMA_ISR 1, 5
DMA_ISR 1, 6
DMA_ISR 1, 7
DMA_ISR 2, 1
DMA_ISR 2, 2
DMA_ISR 2, 3
DMA_ISR 2, 4
DMA_ISR 2, 5
