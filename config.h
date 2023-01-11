#pragma once

#define F_CPU 72000000
#define CNF_SCHEDULER_TICK_MS 10

/*
 * If display power is controlled by transistor switch, the gpio port and pin
 * number, that controls transistor base  will be defined then
 */
#define CNF_DSPL_PWR 1
/* GPIO port letter */
#define CNF_DSPL_PWR_PORT A
/* GPIO port pin */
#define CNF_DSPL_PWR_PIN 12

/* I2C runs based on interrupts */
#define CONFIG_I2C_ASYNC 1

#define CNF_BUTTON_KEY_LEFT_PORT B
#define CNF_BUTTON_KEY_MID_PORT B
#define CNF_BUTTON_KEY_RIGHT_PORT B
#define CNF_BUTTON_KEY_LEFT_PIN 9
#define CNF_BUTTON_KEY_MID_PIN 8
#define CNF_BUTTON_KEY_RIGHT_PIN 5

/* Debug pin */
#define CNF_DEBUG_PIN_PIN 13
#define CNF_DEBUG_PIN_PORT C
