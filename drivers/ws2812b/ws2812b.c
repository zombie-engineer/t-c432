#include "common_util.h"
#include <config.h>
#include <gpio.h>
#include <stdint.h>
#include <dma.h>
#include <tim.h>
#include <svc.h>

/*
 * According to datasheet reset signal should be 50us
 * 50us / 1.25 = 40
 */
#define PWM_FIRST_CCR_AS_RESET 1
#define NUM_RESET_BYTES (40 - PWM_FIRST_CCR_AS_RESET)
#define NUM_LEDS 44

static uint8_t led_strip_buf[NUM_LEDS * 8 * 3 + NUM_RESET_BYTES];

static const uint8_t pwm_low = (0.4f / 1.25f) * LED_TIM_PERIOD + 1;
static const uint8_t pwm_high = (0.8f / 1.25f) * LED_TIM_PERIOD + 1;

static uint8_t *led_strip_put_byte(uint8_t *dst, uint8_t value)
{
  for (int i = 0; i < 8; ++i) {
    int bit = (value >> (7-i)) & 1;
    if (bit)
      *dst++ = pwm_high;
    else
      *dst++ = pwm_low;
  }
  return dst;
}

static uint8_t *led_strip_put_pixel(uint8_t *dst, uint8_t r, uint8_t g, uint8_t b)
{
  dst = led_strip_put_byte(dst, g);
  dst = led_strip_put_byte(dst, r);
  dst = led_strip_put_byte(dst, b);
  return dst;
}

struct hobo_led {
  int idx;
  int time_to_go;
  int rest_time;
};

struct hobo_led hleds[3];

static void hobo_led_update(struct hobo_led *l)
{
  if (l->time_to_go)
    l->time_to_go--;

  if (!l->time_to_go) {
    l->time_to_go = l->rest_time;
    l->idx++;
    if (l->idx == NUM_LEDS)
      l->idx = 0;
  }
}

static void hobo_leds_init(void)
{
  hleds[0].idx = 3;
  hleds[0].time_to_go = 0;
  hleds[0].rest_time = 5;
  hleds[1].idx = 6;
  hleds[1].time_to_go = 0;
  hleds[1].rest_time = 17;
  hleds[2].idx = 9;
  hleds[2].time_to_go = 0;
  hleds[2].rest_time = 200;
}

static void hobo_leds_update(void)
{
  for (int i = 0; i < ARRAY_SIZE(hleds); ++i) {
    hobo_led_update(&hleds[i]);
  }
}

static void hobo_led_draw(struct hobo_led *l, int led_id)
{
  uint8_t *p = led_strip_buf + l->idx * 8 * 3;
  int r;
  int g;
  int b;
  switch(led_id)
  {
  case 0:
    r = 255; g = 0; b = 0;
    break;
  case 1:
    r = 235; g = 0; b = 10;
    break;
  case 2:
    r = 215; g = 0; b = 10;
    break;
  }
  led_strip_put_pixel(p, r, g, b);
}

static void hobo_leds_draw(void)
{
  for (int i = 0; i < ARRAY_SIZE(hleds); ++i) {
    hobo_led_draw(&hleds[i], i);
  }
}

static int dma_channel;

static void on_dma_finished(void)
{
  uint8_t *p;
  if (dma_channel == -1)
    svc_call(SVC_PANIC);

  tim3_disable();
  dma_transfer_disable(dma_channel);
  hobo_leds_update();

  for (int i = 0; i < NUM_LEDS; ++i) {
    uint8_t *p = led_strip_buf + i * 8 * 3;
    float coeff = (float)i / NUM_LEDS;
    led_strip_put_pixel(p, 1, 0, coeff * 8 + 1);
  }

  hobo_leds_draw();

  tim3_pwm_dma_run(dma_channel, led_strip_buf, sizeof(led_strip_buf));
}

static void ws2812b_fill_buf(void)
{
  uint8_t *p = led_strip_buf;

  for (int pix = 0; pix < NUM_LEDS; ++pix) {
    if (pix == 10 || pix == 20)
      p = led_strip_put_pixel(p, 255, 0, 0);
    else if (pix % 2)
      p = led_strip_put_pixel(p, 0, 16, 0);
    else
      p = led_strip_put_pixel(p, 0, 0, 30);
  }

  for (int i = 0; i < NUM_RESET_BYTES; ++i)
    *p++ = 0;
}

void ws2812b_init(void)
{
  ws2812b_fill_buf();
  hobo_leds_init();
  gpio_setup(GPIO_PORT_B, 0, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
  tim3_setup(1, LED_TIM_PERIOD - 1);
  dma_init();

  tim3_pwm_dma_setup(TIM3_CH3);
  tim3_enable();

  dma_channel = dma_get_channel_id(DMA_PERIPH_TIM3_CH3);
  dma_enable_interrupt(DMA_NUM_1, dma_channel);
  dma_set_isr_cb(DMA_NUM_1, dma_channel, on_dma_finished);
  tim3_pwm_dma_run(dma_channel, led_strip_buf, sizeof(led_strip_buf));
  while(1);
}

void ws2812b_update(void)
{
}
