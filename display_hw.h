#pragma once

#define DISPLAY_DRIVER_SSD1306 1
#define DISPLAY_DRIVER_SH1106 2
#define DISPLAY_DRIVER DISPLAY_DRIVER_SH1106

#if DISPLAY_DRIVER == DISPLAY_DRIVER_SSD1306
#include "ssd1306.h"
#define display_hw_init ssd1306_init
#define display_hw_flush ssd1306_flush
#define display_hw_get_size ssd1306_get_size

#elif DISPLAY_DRIVER == DISPLAY_DRIVER_SH1106
#include "drivers/sh1106/sh1106.h"
#define display_hw_init sh1106_init
#define display_hw_flush sh1106_flush
#define display_hw_get_size sh1106_get_size
#endif


