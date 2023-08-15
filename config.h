#pragma once

#define F_CPU 72000000
#define F_SYSCLOCK F_CPU
#define F_APB1 (F_CPU / 2)
#define F_APB2 F_CPU

#define CNF_SCHEDULER_TICK_MS 10

/*
 * If display power is controlled by transistor switch, the gpio port and pin
 * number, that controls transistor base  will be defined then
 */
#define CNF_DSPL_PWR 1
/* GPIO port letter */
#define CNF_DSPL_PWR_PORT A
/* GPIO port pin */
#define CNF_DSPL_PWR_PIN 10

/* I2C runs based on interrupts */
#define CONFIG_I2C_ASYNC 1

#define LED_TIM_PERIOD 45

/* ADC has internal temperature sensor. Enable it */
#define ADC_INT_TEMP_SENSOR_ENABLE 1

#define ADC_CH_TEMP_SENSOR0 ADC_CH_0
#define ADC_CH_TEMP_SENSOR1 ADC_CH_1
#define ADC_CH_TEMP_SENSOR2 ADC_CH_16
#define ADC_CH_VREF ADC_CH_17
