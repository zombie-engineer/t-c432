#pragma once

#define NVIC_BASE 0xe000e100
/*
 * Interrupt set-enable registers.
 * On read shows which interrupts are enabled.
 * On writing 1 enables interrupt.
 */
#define NVIC_ISER0 (volatile uint32_t *)(NVIC_BASE + 0x00)
#define NVIC_ISER1 (volatile uint32_t *)(NVIC_BASE + 0x04)
#define NVIC_ISER2 (volatile uint32_t *)(NVIC_BASE + 0x08)

/*
 * Interrupt clear-enable registers.
 * On read shows which interrupts are enabled.
 * On writing 1 disables interrupt.
 */
#define NVIC_ICER0 (volatile uint32_t *)(NVIC_BASE + 0x80)
#define NVIC_ICER1 (volatile uint32_t *)(NVIC_BASE + 0x84)
#define NVIC_ICER2 (volatile uint32_t *)(NVIC_BASE + 0x88)

/*
 * Interrupt set-pending registers.
 * On read shows which interrupts are pending.
 * On writing 1 set pending status.
 */
#define NVIC_ISPR0 (volatile uint32_t *)(NVIC_BASE + 0x100)
#define NVIC_ISPR1 (volatile uint32_t *)(NVIC_BASE + 0x104)
#define NVIC_ISPR2 (volatile uint32_t *)(NVIC_BASE + 0x108)

/*
 * Interrupt clear-pending registers.
 * On read shows which interrupts are pending.
 * On writing 1 clears pending status.
 */
#define NVIC_ICPR0 (volatile uint32_t *)(NVIC_BASE + 0x180)
#define NVIC_ICPR1 (volatile uint32_t *)(NVIC_BASE + 0x184)
#define NVIC_ICPR2 (volatile uint32_t *)(NVIC_BASE + 0x188)

/* Interrupt active bits registers */
#define NVIC_IABR0 (volatile uint32_t *)(NVIC_BASE + 0x200)
#define NVIC_IABR1 (volatile uint32_t *)(NVIC_BASE + 0x204)
#define NVIC_IABR2 (volatile uint32_t *)(NVIC_BASE + 0x208)

/* Interrupt priority registers */
#define NVIC_IPR0 (volatile uint32_t *)(NVIC_BASE + 0x300)
#define NVIC_STIR (volatile uint32_t *)(NVIC_BASE + 0xe00)


#define NVIC_INTERRUPT_NUMBER_ADC1 18
#define NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX 19
#define NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0 20
#define NVIC_INTERRUPT_NUMBER_TIM2 28
#define NVIC_INTERRUPT_NUMBER_USB_WAKEUP 42

