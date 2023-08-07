#include "main_task.h"
#include "systick.h"
#include <ui.h>
#include "adc.h"
#include <tim.h>
#include <math.h>
#include <rcc.h>
#include <usb_driver.h>
#include <gpio.h>
#include <svc.h>
#include <config.h>
#include "drivers/ws2812b/ws2812b.h"
#include "debug_pin.h"
#include "common_util.h"
#include "ntc10k.h"
#include <stdlib.h>
#include "pin_config.h"
#include "pushbuttons.h"

void timer_setup(void)
{
  /* SYSCLK = 72MHz */
  rcc_periph_ena(RCC_PERIPH_TIM2);
  tim2_setup(true, CALC_PSC(0.1, F_CPU, 0xffff), 0xffff, true, true);
}

void usb_set_address_callback(void *arg)
{
  ui_on_set_address();
}

void usb_tx_callback(void *arg)
{
  ui_on_tx(arg);
}

void usb_rx_callback(void *arg)
{
  ui_on_rx(arg);
}

#define TEMP_SENSOR_ARRAY_LENGTH 128
uint16_t temp_sensor_array[TEMP_SENSOR_ARRAY_LENGTH];

#define THERMOSTAT_STATE_COOLING 0
#define THERMOSTAT_STATE_HEATING_ACTIVE 1
#define THERMOSTAT_STATE_HEATING_INERTIAL 2

#define THERMOSTAT_SETPOINT_CELSIUS 35
#define TEMP_SENSOR_ADC_CHANNEL 0

#define PUMP_STATE_IDLING  0
#define PUMP_STATE_WORKING 1

int thermostat_state = THERMOSTAT_STATE_COOLING;
int pump_state = PUMP_STATE_IDLING;

int termo_force_heating_timer = 0;

int pump_process_counter = 0;

static bool button_is_pressed = false;

static void pump_enable(void)
{
  gpio_odr_modify(PUMP_ENABLE_GPIO_PORT, PUMP_ENABLE_GPIO_PIN, 1);
}

static void pump_disable(void)
{
  gpio_odr_modify(PUMP_ENABLE_GPIO_PORT, PUMP_ENABLE_GPIO_PIN, 0);
}

static void pump_gpio_init(void)
{
  gpio_setup(PUMP_ENABLE_GPIO_PORT, PUMP_ENABLE_GPIO_PIN,
    GPIO_MODE_OUT_10_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);

  pump_disable();
}

static void thermostat_heater_init(void)
{
  gpio_setup(THERMOSTAT_ENABLE_GPIO_PORT, THERMOSTAT_ENABLE_GPIO_PIN,
    GPIO_MODE_OUT_10_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);

  gpio_odr_modify(THERMOSTAT_ENABLE_GPIO_PORT, THERMOSTAT_ENABLE_GPIO_PIN, 1);
}

static void thermostat_heater_enable(void)
{
  debug_pin_off();
  gpio_odr_modify(THERMOSTAT_ENABLE_GPIO_PORT, THERMOSTAT_ENABLE_GPIO_PIN, 1);
}

static void thermosat_heater_disable(void)
{
  debug_pin_on();
  gpio_odr_modify(THERMOSTAT_ENABLE_GPIO_PORT, THERMOSTAT_ENABLE_GPIO_PIN, 0);
}

static uint16_t temp_sensor_get_filtered(void)
{
  uint32_t sum = 0;
  for (int i = 0; i < ARRAY_SIZE(temp_sensor_array); ++i)
    sum += temp_sensor_array[i];

  return (uint16_t)(sum / ARRAY_SIZE(temp_sensor_array));
}

static uint32_t temp_sensor_adc_to_resistance(uint16_t adc_value)
{
  const int adc_value_width = 12;
  const float r2 = 5100.0f;
  const float vref = 3.3f;
  const float adc_max_value = (float)(1 << adc_value_width);
  float v_measured = adc_value / adc_max_value * vref;
  return r2 * v_measured / (vref - v_measured);
}

static float temp_sensor_calc_degrees_celisus(void)
{
  uint16_t adc_filtered;
  uint32_t measured_resistance;

  adc_filtered = temp_sensor_get_filtered();
  measured_resistance = temp_sensor_adc_to_resistance(adc_filtered);
  return ntc10k_3950_resistance_to_degree_celsius(measured_resistance);
}

float temperature_celsius = 0;

static void thermostat_run(void)
{
  const int setpoint_temperature_celsius = THERMOSTAT_SETPOINT_CELSIUS;
  int temperature_celsius_int;

  temperature_celsius = temp_sensor_calc_degrees_celisus();
  temperature_celsius_int = roundf(temperature_celsius);

  if (thermostat_state == THERMOSTAT_STATE_COOLING) {
    if (temperature_celsius < setpoint_temperature_celsius) {
      thermostat_state = THERMOSTAT_STATE_HEATING_ACTIVE;
      thermostat_heater_enable();
      termo_force_heating_timer = 100;
    }
    else
    {
      thermosat_heater_disable();
    }
  } else if (thermostat_state == THERMOSTAT_STATE_HEATING_ACTIVE) {
    termo_force_heating_timer--;
    if (termo_force_heating_timer <= 0) {
      thermosat_heater_disable();
      thermostat_state = THERMOSTAT_STATE_HEATING_INERTIAL;
      termo_force_heating_timer = 1200;
    }
  } else if (thermostat_state == THERMOSTAT_STATE_HEATING_INERTIAL) {
    termo_force_heating_timer--;
    if (termo_force_heating_timer <= 0) {
      thermostat_state = THERMOSTAT_STATE_COOLING;
      thermosat_heater_disable();
    }
  }
}

static void thermostat_init(void)
{
  thermostat_heater_init();
  adc_setup_freerunning_dma(TEMP_SENSOR_GPIO_PORT, TEMP_SENSOR_GPIO_PIN,
    temp_sensor_array, ARRAY_SIZE(temp_sensor_array));
}

#define PUMP_BUTTON_IGNORE_TIME 30

static void pump_button_event(int button_id, int event)
{
  if (button_id != PUSHBUTTON_ID_MAIN)
    return;

  if (event == PUSHBUTTON_EVENT_PRESSED)
    button_is_pressed = true;
  else
    button_is_pressed = false;
}

static void flow_meter_init(void)
{
  gpio_setup(FLOW_METER_GPIO_PORT, FLOW_METER_GPIO_PIN,
    GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
}

static void pump_init(void)
{
  pump_gpio_init();

  pushbutton_register_callback(PUSHBUTTON_ID_MAIN, PUSHBUTTON_EVENT_PRESSED,
    pump_button_event);

  pushbutton_register_callback(PUSHBUTTON_ID_MAIN, PUSHBUTTON_EVENT_RELEASED,
    pump_button_event);

  pump_process_counter = PUMP_BUTTON_IGNORE_TIME;
}

static void pump_run(void)
{
  if (pump_process_counter > 0) {
    pump_process_counter--;
    return;
  }
  pump_process_counter = PUMP_BUTTON_IGNORE_TIME;

  if (pump_state == PUMP_STATE_WORKING) {
    if (!button_is_pressed) {
      pump_disable();
      pump_state = PUMP_STATE_IDLING;
    }
  } else {
    if (button_is_pressed) {
      pump_enable();
      pump_state = PUMP_STATE_WORKING;
    }
  }
}

void main_task_fn(void *)
{
  ui_set_adcbuf(temp_sensor_array, ARRAY_SIZE(temp_sensor_array));
  systick_set(CNF_SCHEDULER_TICK_MS);
  timer_setup();

  thermostat_init();
  pump_init();

  while(1) {
    thermostat_run();
    pump_run();
    svc_wait_ms(10);
  }
}
