#include "ntc10k.h"
#include <math.h>

/*
 * Resistance to temperatures table taken from:
 *   https://rgp-tech.ru/rt-table/
 *
 * Temperature formulas are ere:
 *   https://www.giangrandi.org/electronics/ntc/ntc.shtml
 *
 * Nice table also here:
 *   https://cdn-shop.adafruit.com/datasheets/103_3950_lookuptable.pdf
 */

float ntc10k_3950_resistance_to_degree_celsius(uint32_t resistance)
{
  const float kelvin_to_celsius_offset = 273.5f;
  const float beta = 3435;// 3950.0f;
  const float r25 = 10000.0f;
  const float recip_t24_kelvin = 1.0f / 298.15f;
  const float step1 = log(resistance / r25);
  const float step2 = step1 / beta + recip_t24_kelvin;
  const float step3 = 1.0f / step2;
  return step3 - kelvin_to_celsius_offset;
}


