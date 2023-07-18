def amps_to_milliamps(amps):
  return amps * 0.001

def calc_led_resistor(v_source, v_led, i_led):
  # v_source is clear
  # v_led is "forward voltage" or "voltage drop" on led
  # i_led - desired current through led
  return (v_source - v_led) / i_led

print(calc_led_resistor(3.3, 1.2, amps_to_milliamps(15)))
print(calc_led_resistor(3.3, 1.2, amps_to_milliamps(14)))
