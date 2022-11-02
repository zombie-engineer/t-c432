#pragma once
#include <stdint.h>

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
#define NVIC_ICER_INTS_PER_REG 32

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

#define NVIC_INTERRUPT_NUMBER_WWDG 0
#define NVIC_INTERRUPT_NUMBER_PVD 1
#define NVIC_INTERRUPT_NUMBER_TAMPER 2
#define NVIC_INTERRUPT_NUMBER_RTC 3
#define NVIC_INTERRUPT_NUMBER_FLASH 4
#define NVIC_INTERRUPT_NUMBER_RCC 5
#define NVIC_INTERRUPT_NUMBER_EXTI0 6
#define NVIC_INTERRUPT_NUMBER_EXTI1 7
#define NVIC_INTERRUPT_NUMBER_EXTI2 8
#define NVIC_INTERRUPT_NUMBER_EXTI3 9
#define NVIC_INTERRUPT_NUMBER_EXTI4 10
#define NVIC_INTERRUPT_NUMBER_DMA1_CHAN_1 11
#define NVIC_INTERRUPT_NUMBER_DMA1_CHAN_2 12
#define NVIC_INTERRUPT_NUMBER_DMA1_CHAN_3 13
#define NVIC_INTERRUPT_NUMBER_DMA1_CHAN_4 14
#define NVIC_INTERRUPT_NUMBER_DMA1_CHAN_5 15
#define NVIC_INTERRUPT_NUMBER_DMA1_CHAN_6 16
#define NVIC_INTERRUPT_NUMBER_DMA1_CHAN_7 17
#define NVIC_INTERRUPT_NUMBER_ADC1 18
#define NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX 19
#define NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0 20
#define NVIC_INTERRUPT_NUMBER_CAN_RX1 21
#define NVIC_INTERRUPT_NUMBER_CAN_SCE 22
#define NVIC_INTERRUPT_NUMBER_EXTI9_5 23
#define NVIC_INTERRUPT_NUMBER_TIM1_BRK 24
#define NVIC_INTERRUPT_NUMBER_TIM1_UP 25
#define NVIC_INTERRUPT_NUMBER_TIM1_TRG_COM 26
#define NVIC_INTERRUPT_NUMBER_TIM1_CC 27
#define NVIC_INTERRUPT_NUMBER_TIM2 28
#define NVIC_INTERRUPT_NUMBER_TIM3 29
#define NVIC_INTERRUPT_NUMBER_TIM4 30
#define NVIC_INTERRUPT_NUMBER_I2C1_EV 31
#define NVIC_INTERRUPT_NUMBER_I2C1_ER 32
#define NVIC_INTERRUPT_NUMBER_I2C2_EV 33
#define NVIC_INTERRUPT_NUMBER_I2C2_ER 34
#define NVIC_INTERRUPT_NUMBER_SPI1 35
#define NVIC_INTERRUPT_NUMBER_SPI2 36
#define NVIC_INTERRUPT_NUMBER_USART1 37
#define NVIC_INTERRUPT_NUMBER_USART2 38
#define NVIC_INTERRUPT_NUMBER_USART3 39
#define NVIC_INTERRUPT_NUMBER_EXTI10_15 40
#define NVIC_INTERRUPT_NUMBER_RTC_ALARM 41
#define NVIC_INTERRUPT_NUMBER_USB_WAKEUP 42
#define NVIC_INTERRUPT_NUMBER_TIM8_BRK 43
#define NVIC_INTERRUPT_NUMBER_TIM8_UP 44
#define NVIC_INTERRUPT_NUMBER_TIM8_TRG_COM 45
#define NVIC_INTERRUPT_NUMBER_TIM8_CC 46
#define NVIC_INTERRUPT_NUMBER_ADC3 47
#define NVIC_INTERRUPT_NUMBER_FSMC 48
#define NVIC_INTERRUPT_NUMBER_SDIO 49
#define NVIC_INTERRUPT_NUMBER_TIM5 50
#define NVIC_INTERRUPT_NUMBER_SPI3 51
#define NVIC_INTERRUPT_NUMBER_UART4 52
#define NVIC_INTERRUPT_NUMBER_UART5 53
#define NVIC_INTERRUPT_NUMBER_TIM6 54
#define NVIC_INTERRUPT_NUMBER_TIM7 55
#define NVIC_INTERRUPT_NUMBER_DMA2_CHAN_1 56
#define NVIC_INTERRUPT_NUMBER_DMA2_CHAN_2 57
#define NVIC_INTERRUPT_NUMBER_DMA2_CHAN_3 58
#define NVIC_INTERRUPT_NUMBER_DMA2_CHAN_4_5 59
#define NVIC_INTERRUPT_NUMBER_MAX NVIC_INTERRUPT_NUMBER_DMA2_CHAN_4_5

uint8_t nvic_get_priority(int interrupt_no);
void nvic_set_priority(int interrupt_no, uint8_t pri);
void nvic_enable_interrupt(int interrupt_no);
