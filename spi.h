#pragma once
#include <stdint.h>

#define SPI_GPIO_PIN_MOSI 1
#define SPI_GPIO_PIN_MISO 2
#define SPI_GPIO_PIN_SCK  4
#define SPI_GPIO_PIN_NSS  8

void spi_init(int spi_pin_mask);

void spi_transfer(const uint8_t *data, int len);
