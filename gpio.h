#pragma once

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

void gpioa_set_cr(int pin_nr, int mode, int cnf);

void gpioc_set_pin13(void);

void gpioa_set_odr(int bit);

void gpioc_bit_clear(int pin_nr);

void gpioc_bit_set(int pin_nr);

void gpiob_set_cr(int pin_nr, int mode, int cnf);
