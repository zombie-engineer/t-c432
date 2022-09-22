#!/bin/python3


# PSC = fCLK * T / ARR - 1

def get_psc(fCLK, T, ARR):
  return float(fCLK) * T / ARR - 1

print('hello', get_psc(8000000.0, 1.0, 0xffff))
