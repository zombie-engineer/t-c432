#pragma once

#define F_CPU 72000000

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

