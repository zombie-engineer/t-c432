#pragma once

typedef enum {
  RCC_SRC_HSI,
  RCC_SRC_HSE,
  RCC_SRC_PLL,
} rcc_src_enum;

void rcc_set_72mhz_usb(void);
void rcc_enable_usb(void);

typedef enum {
  RCC_PERIPH_DMA1,
  RCC_PERIPH_DMA2,
  RCC_PERIPH_SRAM,
  RCC_PERIPH_FLIT,
  RCC_PERIPH_CRC,
  RCC_PERIPH_OTGFS,
  RCC_PERIPH_ETHMAC,
  RCC_PERIPH_ETHMACTX,
  RCC_PERIPH_ETHMACRX,
  RCC_PERIPH_TIM2,
  RCC_PERIPH_TIM3,
  RCC_PERIPH_TIM4,
  RCC_PERIPH_TIM5,
  RCC_PERIPH_TIM6,
  RCC_PERIPH_TIM7,
  RCC_PERIPH_WWDG,
  RCC_PERIPH_SPI2,
  RCC_PERIPH_SPI3,
  RCC_PERIPH_USART2,
  RCC_PERIPH_USART3,
  RCC_PERIPH_USART4,
  RCC_PERIPH_USART5,
  RCC_PERIPH_I2C1,
  RCC_PERIPH_I2C2,
  RCC_PERIPH_USB,
  RCC_PERIPH_CAN1,
  RCC_PERIPH_CAN2,
  RCC_PERIPH_BKP,
  RCC_PERIPH_PWR,
  RCC_PERIPH_DAC,
  RCC_PERIPH_AFIO,
  RCC_PERIPH_IOPA,
  RCC_PERIPH_IOPB,
  RCC_PERIPH_IOPC,
  RCC_PERIPH_IOPD,
  RCC_PERIPH_IOPE,
  RCC_PERIPH_ADC1,
  RCC_PERIPH_ADC2,
  RCC_PERIPH_TIM1,
  RCC_PERIPH_SPI1,
  RCC_PERIPH_USART1,
} rcc_clock_periph_t;

/* preprocessor extend macro intermediate step */
#define __RCC_GPIO_PORT(__letter) RCC_PERIPH_IOP ## __letter

/*
 * preprocessor convert letter A, B, C to RCC_PERIPH_IOPA, RCC_PERIPH_IOPB,
 * etc. Additional preprocessing step is needed to convert possible config
 * definition to actual letter. See config.h
 */
#define RCC_GPIO_PORT(__letter) \
  __RCC_GPIO_PORT(__letter)

/* Enable peripheral */
void rcc_periph_ena(rcc_clock_periph_t p);

/* Disable peripheral */
void rcc_periph_disa(rcc_clock_periph_t p);
