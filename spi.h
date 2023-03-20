#pragma once
#include <stdint.h>

void spi_init(void);

void spi_transfer(const uint8_t *data, int len);
