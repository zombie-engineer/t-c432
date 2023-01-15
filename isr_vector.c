#include "compiler.h"
#include "cpu_arm_stm32f103.h"
#include "scb.h"
#include "nvic.h"

extern void reset_isr(void);
extern void nmi_isr(void);
extern void systick_isr(void);
extern void hardfault_isr(void);
extern void memfault_isr(void);
extern void bus_fault_isr(void);
extern void usgfault_isr(void);
extern void svcall_isr(void);
extern void tim2_isr(void);
extern void generic_isr(void);
extern void adc_isr(void);
extern void usb_hp_isr(void);
extern void usb_lp_isr(void);
extern void usb_wakeup_isr(void);
extern void reserved_isr(void);
extern void exti_0_isr(void);
extern void exti_1_isr(void);
extern void exti_2_isr(void);
extern void exti_3_isr(void);
extern void exti_4_isr(void);
extern void exti_9_5_isr(void);
extern void exti_15_10_isr(void);
extern void dma1_ch1_isr(void);
extern void dma1_ch2_isr(void);
extern void dma1_ch3_isr(void);
extern void dma1_ch4_isr(void);
extern void dma1_ch5_isr(void);
extern void dma1_ch6_isr(void);
extern void dma1_ch7_isr(void);
extern void i2c1_ev_isr(void);
extern void i2c1_er_isr(void);

__attribute__((section(".isr_vector")))

#define STACK_START 0x20006000
#define NUM_EXCEPTIONS (16 + 60)

#define ISR(__i, __fn, __is_thumb) \
  [__i] = ((int)__fn) + __is_thumb

int isr_vector[] = {
  ISR(0, STACK_START, 0),
  ISR(1, reset_isr, 1),
  ISR(2, nmi_isr, 1),
  ISR(3, hardfault_isr, 1),
  ISR(4, memfault_isr, 1),
  ISR(5, bus_fault_isr, 1),
  ISR(6, usgfault_isr, 1),
  ISR(7, reserved_isr, 1),
  ISR(8, reserved_isr, 1),
  ISR(9, reserved_isr, 1),
  ISR(10, reserved_isr, 1),
  ISR(11, svcall_isr, 1),
  ISR(12, generic_isr, 1),
  ISR(13, generic_isr, 1),
  ISR(14, generic_isr, 1),
  ISR(15, systick_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_EXTI0, exti_0_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_EXTI1, exti_1_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_EXTI2, exti_2_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_EXTI3, exti_3_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_EXTI4, exti_4_isr, 1),

  ISR(16 + NVIC_INTERRUPT_NUMBER_DMA1_CHAN_1, dma1_ch1_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_DMA1_CHAN_2, dma1_ch2_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_DMA1_CHAN_3, dma1_ch3_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_DMA1_CHAN_4, dma1_ch4_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_DMA1_CHAN_5, dma1_ch5_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_DMA1_CHAN_6, dma1_ch6_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_DMA1_CHAN_7, dma1_ch7_isr, 1),

  ISR(16 + NVIC_INTERRUPT_NUMBER_ADC1, adc_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX, usb_hp_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0, usb_lp_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_TIM2, tim2_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_USB_WAKEUP, usb_wakeup_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_EXTI9_5, exti_9_5_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_EXTI15_10, exti_15_10_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_I2C1_EV, i2c1_ev_isr, 1),
  ISR(16 + NVIC_INTERRUPT_NUMBER_I2C1_ER, i2c1_er_isr, 1),

};

void hardfault_isr(void)
{
  uint32_t v = scb_get_shcsr();
  bool is_vector_table_hardfault = scb_is_vector_table_hardfault();
  BRK;
}

void bus_fault_isr(void) { }
void generic_isr(void) { }
void nmi_isr(void) {}

