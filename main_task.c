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
#include "temp_info.h"

#define THERMOSTAT_PWM_PERIOD 100
struct thermostat_pwm {
  int off_time;
  int on_time;
  int counter;
};

static struct thermostat_pwm current_pwm;
void timer_setup(void)
{
  /* SYSCLK = 72MHz */
  rcc_periph_ena(RCC_PERIPH_TIM2);
  // tim2_setup(true, CALC_PSC(0.1, F_CPU, 0xffff), 0xffff, true, true);
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

#define THERMOSTAT_STATE_COOLING 0
#define THERMOSTAT_STATE_HEATING_ACTIVE 1
#define THERMOSTAT_STATE_HEATING_INACTIVE 2

#define THERMOSTAT_SETPOINT_CELSIUS 45.0f
#define THERMOSTAT_HYSTERESIS 1.0f

#define ADC_NUM_CHANNELS 1
#define ADC_NUM_FILTER_SAMPLES_LOG2 7
#define ADC_NUM_FILTER_SAMPLES (1 << ADC_NUM_FILTER_SAMPLES_LOG2)
#define ADC_VREF 3.3f
#define ADC_MAX_VALUE 4096
#define TEMP_HISTORY_DEPTH 256

#define ALL_SENSORS_BUF_LEN (ADC_NUM_CHANNELS * ADC_NUM_FILTER_SAMPLES)
static int adc_dma_buffer_idx = 0;
uint16_t adc_dma_buffer[ALL_SENSORS_BUF_LEN];

static struct temp_info temp_history[TEMP_HISTORY_DEPTH] = { 0 };

static int adc_voltage_history_idx = 0;

static float current_temp = 0.0f;

#define TEMP_SENSOR_0_R1 5100.0f // 4800.0f // 5100.0f
#define TEMP_SENSOR_1_R2 95300.0f // 100000.0f
#define TEMP_SENSOR_VREF 3.25f

static uint32_t temp_sensor_voltage_to_resistance_2(float voltage, float r1)
{
  return r1 * voltage / (TEMP_SENSOR_VREF - voltage);
}

static uint32_t temp_sensor_voltage_to_resistance_1(float voltage, float r2)
{
  return r2 * (TEMP_SENSOR_VREF - voltage) / voltage;
}

static float adc_voltage_to_temp0(float voltage)
{
  uint32_t resist = temp_sensor_voltage_to_resistance_2(voltage,
    TEMP_SENSOR_0_R1);

  ntc10k_3950_resistance_to_degree_celsius(resist);
}

static float adc_voltage_to_temp1(float voltage)
{
  uint32_t resist = temp_sensor_voltage_to_resistance_1(voltage,
    TEMP_SENSOR_1_R2);

  ntc100k_3950_resistance_to_degree_celsius(resist);
}

static int temp_history_counter = 0;

static void temp_history_update(float temp)
{
  int i;

  temp_history[0].temp = temp;

  if (!temp_history_counter) {
    for (i = ARRAY_SIZE(temp_history) - 1; i > 0; i--)
      temp_history[i] = temp_history[i - 1];

    temp_history_counter = 40;
    return;
  }

  temp_history_counter--;
}

static void adc_apply_filtering(void)
{
  uint32_t avg_values[ADC_NUM_CHANNELS] = { 0 };
  uint32_t sample_idx;
  uint32_t ch_idx;

  for (sample_idx = 0; sample_idx < ADC_NUM_FILTER_SAMPLES; ++sample_idx) {
    for (ch_idx = 0; ch_idx < ADC_NUM_CHANNELS; ++ch_idx) {
      uint32_t src_idx = sample_idx * ADC_NUM_CHANNELS + ch_idx;
      avg_values[ch_idx] += adc_dma_buffer[src_idx];
    }
  }
  float avg_voltage = avg_values[0] / (float)ADC_NUM_FILTER_SAMPLES;
  float voltage = avg_voltage / ADC_MAX_VALUE * ADC_VREF;
  current_temp = adc_voltage_to_temp1(voltage);
  temp_history_update(current_temp);
  ui_set_current_temp(current_temp);
}

#define PUMP_STATE_IDLING  0
#define PUMP_STATE_WORKING 1

int thermostat_state = THERMOSTAT_STATE_COOLING;
int pump_state = PUMP_STATE_IDLING;

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
  pump_disable();
}

static void thermostat_heater_enable(void)
{
  debug_pin_off();
  gpio_odr_modify(THERMOSTAT_ENABLE_GPIO_PORT, THERMOSTAT_ENABLE_GPIO_PIN, 1);
}

static void thermostat_heater_disable(void)
{
  debug_pin_on();
  gpio_odr_modify(THERMOSTAT_ENABLE_GPIO_PORT, THERMOSTAT_ENABLE_GPIO_PIN, 0);
}

static void thermostat_heater_init(void)
{
  thermostat_heater_disable();
}

static uint16_t temp_sensor_get_filtered(void)
{
  uint32_t sum = 0;
  for (int i = 0; i < ARRAY_SIZE(adc_dma_buffer); ++i)
    sum += adc_dma_buffer[i];

  return (uint16_t)(sum / ARRAY_SIZE(adc_dma_buffer));
}

static void thermostat_pwm_setup(struct thermostat_pwm *p)
{
  float delta;

  /* Proportional */
  delta = THERMOSTAT_SETPOINT_CELSIUS - current_temp;
  if (delta < 0.0f)
    delta = 0.0f;
  float on_time_coeff = (delta / 40.0) * 0.7;
  p->on_time = (int)(on_time_coeff * THERMOSTAT_PWM_PERIOD);

  if (p->on_time > THERMOSTAT_PWM_PERIOD)
    p->on_time = THERMOSTAT_PWM_PERIOD;

  p->off_time = THERMOSTAT_PWM_PERIOD - p->on_time;
  p->counter = p->on_time;
}

static void thermostat_enter_state_heating_active(void)
{
  thermostat_pwm_setup(&current_pwm);
  thermostat_heater_enable();
  thermostat_state = THERMOSTAT_STATE_HEATING_ACTIVE;
}

static void thermostat_enter_state_heating_inactive(void)
{
  thermostat_heater_disable();
  thermostat_state = THERMOSTAT_STATE_HEATING_INACTIVE;
  current_pwm.counter = current_pwm.off_time;
}

static void thermostat_enter_state_cooling(void)
{
  thermostat_heater_disable();
  thermostat_state = THERMOSTAT_STATE_COOLING;
}

static void thermostat_run(void)
{
  if (thermostat_state == THERMOSTAT_STATE_COOLING) {
    if (current_temp < THERMOSTAT_SETPOINT_CELSIUS - THERMOSTAT_HYSTERESIS)
      thermostat_enter_state_heating_active();

  } else if (thermostat_state == THERMOSTAT_STATE_HEATING_ACTIVE) {
    current_pwm.counter--;
    if (current_pwm.counter <= 0)
      thermostat_enter_state_heating_inactive();
    else if (current_temp >= THERMOSTAT_SETPOINT_CELSIUS - THERMOSTAT_HYSTERESIS / 2)
      thermostat_enter_state_cooling();

  } else if (thermostat_state == THERMOSTAT_STATE_HEATING_INACTIVE) {
    current_pwm.counter--;
    if (current_pwm.counter <= 0)
      thermostat_enter_state_heating_active();
    else if (current_temp >= THERMOSTAT_SETPOINT_CELSIUS)
      thermostat_enter_state_cooling();
  }
}

static void thermostat_init(void)
{
  current_pwm.off_time = THERMOSTAT_PWM_PERIOD;
  current_pwm.on_time = THERMOSTAT_PWM_PERIOD;
  current_pwm.counter = 0;

  ui_set_target_temp(THERMOSTAT_SETPOINT_CELSIUS);
  thermostat_heater_init();
  thermostat_state = THERMOSTAT_STATE_COOLING;
  adc_setup_dma(adc_dma_buffer, ARRAY_SIZE(adc_dma_buffer));
  adc_run();
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
}

static void pump_init(void)
{
  pump_gpio_init();
  flow_meter_init();

  pushbutton_register_callback(PUSHBUTTON_ID_MAIN, PUSHBUTTON_EVENT_PRESSED,
    pump_button_event);

  pushbutton_register_callback(PUSHBUTTON_ID_MAIN, PUSHBUTTON_EVENT_RELEASED,
    pump_button_event);

  pump_process_counter = PUMP_BUTTON_IGNORE_TIME;
}

int pulse_counter = 0;

static void pump_run(void)
{
  pulse_counter += tim2_read_counter_value();
  ui_set_pulse_count(pulse_counter);

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
  ui_set_temp_history(temp_history, TEMP_HISTORY_DEPTH);
  systick_set(CNF_SCHEDULER_TICK_MS);
  timer_setup();

  thermostat_init();
  pump_init();

  while(1) {
    adc_apply_filtering();
    thermostat_run();
    pump_run();
    svc_wait_ms(10);
  }
}
