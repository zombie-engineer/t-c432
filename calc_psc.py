#!/bin/python3

NANO_SEC = 0
MICRO_SEC = 1
MILLI_SEC = 2
SEC = 3

# PSC = fCLK * T / ARR - 1

def get_prescaler(req_period_sec, f_clk, counts_per_period):
  return float(f_clk) * req_period_sec / counts_per_period

def get_auto_reload_value(req_period_sec, f_clk, prescaler):
  return float(f_clk) * req_period_sec / prescaler


f_clk = 72 * 1000 * 1000

prescaler_value = get_prescaler(f_clk, 0.00000125, 50)
arr_value = get_auto_reload_value(f_clk, 0.00000125, 1)
print('hello', 72 * 1000 * 1000 * 0.00000125 / 0x10, prescaler_value, arr_value)

def freq_to_period(frequency):
  return 1.0 / frequency

def print_time(sec_value, unit):
  if unit == NANO_SEC:
    print('{} nsec'.format(sec_value * 1.0e+9))
  elif unit == MICRO_SEC:
    print('{:.03} usec'.format(sec_value * 1.0e+6))
  elif unit == MILLI_SEC:
    print('{} msec'.format(sec_value * 1.0e+3))
  elif unit == SEC:
    print('{:.03} sec'.format(sec_value))
  else:
    print('<unknown unit>')


print_time(freq_to_period(800 * 1000), MICRO_SEC)



