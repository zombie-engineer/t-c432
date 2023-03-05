#include <config.h>
#include <gpio.h>
#include <stdint.h>

extern void led_strip_timer_setup(const void *mem, int size);

/*
 * According to datasheet reset signal should be 50us
 * 50us / 1.25 = 40
 */
#define PWM_FIRST_CCR_AS_RESET 1
#define NUM_RESET_BYTES (40 - PWM_FIRST_CCR_AS_RESET)
#define NUM_LEDS 28

uint16_t led_strip_buf[NUM_LEDS * 8 * 3 + NUM_RESET_BYTES];

static const uint16_t pwm_low = (0.4f / 1.25f) * LED_TIM_PERIOD + 1;
static const uint16_t pwm_high = (0.8f / 1.25f) * LED_TIM_PERIOD + 1;

static uint16_t *led_strip_put_byte(uint16_t *dst, uint8_t value)
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

static uint16_t *led_strip_put_pixel(uint16_t *dst, uint8_t r, uint8_t g, uint8_t b)
{
  dst = led_strip_put_byte(dst, g);
  dst = led_strip_put_byte(dst, r);
  dst = led_strip_put_byte(dst, b);
  return dst;
}

void run_led_strip(void)
{
  uint16_t *p = led_strip_buf;
  for (int i = 0; i < NUM_RESET_BYTES; ++i) *p++ = 0;

  for (int pix = 0; pix < NUM_LEDS; ++pix) {
    if (pix % 3) {
      float norm = (float)pix / NUM_LEDS;
      p = led_strip_put_pixel(p, 0, 0, norm * 255);
    } else {
      p = led_strip_put_pixel(p, 0, 200, 0);
    }
  }

  gpio_setup(GPIO_PORT_B, 0, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
  led_strip_timer_setup(led_strip_buf, sizeof(led_strip_buf));
  while(1);
}

