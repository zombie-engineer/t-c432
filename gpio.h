#pragma once
#include <stdbool.h>

#define GPIO_PORT_A 0
#define GPIO_PORT_B 1
#define GPIO_PORT_C 2
#define GPIO_PORT_D 3
#define GPIO_PORT_E 4
#define GPIO_PORT_F 5
#define GPIO_PORT_G 6

/* preprocessor extend macro intermediate step */
#define __GPIO_PORT(__letter) GPIO_PORT_ ## __letter

/*
 * preprocessor convert letter A, B, C to RCC_PERIPH_IOPA, RCC_PERIPH_IOPB,
 * etc. Additional preprocessing step is needed to convert possible config
 * definition to actual letter. See config.h
 */
#define GPIO_PORT(__letter) \
  __GPIO_PORT(__letter)

#define GPIO_MODE_INPUT      0b00
#define GPIO_MODE_OUT_10_MHZ 0b01
#define GPIO_MODE_OUT_2_MHZ  0b10
#define GPIO_MODE_OUT_50_MHZ 0b11

#define GPIO_CNF_IN_ANALOG          0b00
#define GPIO_CNF_IN_FLOATING        0b01
#define GPIO_CNF_IN_PULLUP_PULLDOWN 0b10
#define GPIO_CNF_IN_INVALID         0b11

#define GPIO_CNF_OUT_GP_PUSH_PULL   0b00
#define GPIO_CNF_OUT_GP_OPEN_DRAIN  0b01
#define GPIO_CNF_OUT_ALT_PUSH_PULL  0b10
#define GPIO_CNF_OUT_ALT_OPEN_DRAIN 0b11

void gpio_setup(int port, int pin, int mode, int cnf);

void gpio_odr_modify(int port, int pin_nr, int set_clear);

void gpioc_set_pin13(void);

void gpioc_bit_clear(int pin_nr);

void gpioc_bit_set(int pin_nr);

void gpiob_set_cr(int pin_nr, int mode, int cnf);

#define GPIO_REMAP_I2C1_PB6_PB7 0
#define GPIO_REMAP_I2C1_PB8_PB9 1
void gpio_remap_i2c1(int mapping);

bool gpiob_pin_is_set(int pin_nr);

void gpio_map_to_exti(int port, int pin_nr);
