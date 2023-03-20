#pragma once
#include <stdint.h>
#include <stdbool.h>

void ssd1306_init(void);
void ssd1306_flush(void);

bool ssd1306_get_frame_size(int *size_x, int *size_y);
