#!/bin/bash

sudo /usr/bin/openocd \
  -f /usr/share/openocd/scripts/interface/stlink-v2.cfg \
  -f /usr/share/openocd/scripts/target/stm32f1x.cfg \
  -f openocd.cfg \
  -c "gdb_port 3333"
