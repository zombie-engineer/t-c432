#!/bin/bash

sudo openocd \
  -f /usr/share/openocd/scripts/interface/stlink-v2.cfg \
  -f /usr/share/openocd/scripts/target/stm32f1x.cfg \
  -c "gdb_port 3333"
