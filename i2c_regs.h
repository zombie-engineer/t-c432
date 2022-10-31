#pragma once
#include "memory_layout.h"

#define I2C_CR1  (volatile uint32_t *)(I2C1_BASE + 0x00)
#define I2C_CR1_PE 0
#define I2C_CR1_SMBUS 1
#define I2C_CR1_SMBTYPE 3
#define I2C_CR1_ENARP 4
#define I2C_CR1_ENPEC 5
#define I2C_CR1_ENGC 6
#define I2C_CR1_NOSTRETCH 7
#define I2C_CR1_START 8
#define I2C_CR1_STOP 9
#define I2C_CR1_ACK 10
#define I2C_CR1_POS 11
#define I2C_CR1_PEC 12
#define I2C_CR1_ALERT 13
#define I2C_CR1_SWRST 15

#define I2C_CR2  (volatile uint32_t *)(I2C1_BASE + 0x04)
#define I2C_CR2_FREQ 0
#define I2C_CR2_FREQ_WIDTH 6
#define I2C_CR2_ITERREN 8
#define I2C_CR2_ITEVTEN 9
#define I2C_CR2_ITBUFEN 10
#define I2C_CR2_DMAEN 11
#define I2C_CR2_LAST 12

#define I2C_OAR1 (volatile uint32_t *)(I2C1_BASE + 0x08)
#define I2C_OAR1_ADD0 0
#define I2C_OAR1_ADD7_1 1
#define I2C_OAR1_ADD7_1_WIDTH 7
#define I2C_OAR1_ADD8_9 8
#define I2C_OAR1_ADD8_9_WIDTH 2
#define I2C_OAR1_ADD_MODE 15

#define I2C_OAR2 (volatile uint32_t *)(I2C1_BASE + 0x0c)
#define I2C_OAR2_ENDUAL 0
#define I2C_OAR2_ADD2_7_1 1
#define I2C_OAR2_ADD2_7_1_WIDTH 7
#define I2C_DR   (I2C1_BASE + 0x10)
#define I2C_DR_DR 0
#define I2C_DR_DR_WIDTH 8
#define I2C_SR1  (volatile uint32_t *)(I2C1_BASE + 0x14)
#define I2C_SR1_SB 0
#define I2C_SR1_ADDR 1
#define I2C_SR1_BTF 2
#define I2C_SR1_ADD10 3
#define I2C_SR1_STOPF 4
#define I2C_SR1_RXNE 6
#define I2C_SR1_TXE 7
#define I2C_SR1_BERR 8
#define I2C_SR1_ARLO 9
#define I2C_SR1_AF 10
#define I2C_SR1_OVR 11
#define I2C_SR1_PECERR 12
#define I2C_SR1_TIMEOUT 14
#define I2C_SR1_SMBALERT 15
#define I2C_SR2  (volatile uint32_t *)(I2C1_BASE + 0x18)
#define I2C_SR2_MSL 0
#define I2C_SR2_BUSY 1
#define I2C_SR2_TRA 2
#define I2C_SR2_GENCALL 4
#define I2C_SR2_SMBDEFAULT 5
#define I2C_SR2_SMBHOST 6
#define I2C_SR2_DUALF 7
#define I2C_SR2_PEC 8
#define I2C_SR2_PEC_WIDTH 8

#define I2C_CCR  (volatile uint32_t *)(I2C1_BASE + 0x1c)
#define I2C_CCR_CCR 0
#define I2C_CCR_CCR_WIDTH 12
#define I2C_CCR_DUTY 14
#define I2C_CCR_FS   15
#define I2C_CCR_FS_SM 0
#define I2C_CCR_FS_FM 1
#define I2C_TRISE (volatile uint32_t *)(I2C1_BASE + 0x20)
