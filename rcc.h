#pragma once

typedef enum {
  RCC_SRC_HSI,
  RCC_SRC_HSE,
  RCC_SRC_PLL,
} rcc_src_enum;

void rcc_set_72mhz_usb(void);
void rcc_enable_usb();
void rcc_enable_gpio_a();
void rcc_enable_afio();
void rcc_enable_adc1();
void rcc_enable_usart2();
void rcc_enable_tim2();
void rcc_enable_gpio_c();
