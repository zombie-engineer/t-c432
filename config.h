#pragma once

#define F_CPU 72000000

/*
 * If display power is controlled by transistor switch, the gpio port and pin
 * number, that controls transistor base  will be defined then
 */
#define DISPLAY_PWR_CTRL 1
#define DISPLAY_PWR_CTRL_GPIO_PORT A
#define DISPLAY_PWR_CTRL_GPIO_PIN 12
