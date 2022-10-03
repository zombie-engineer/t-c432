#include "i2c.h"
#include "memory_layout.h"
#include "reg_access.h"

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

void b(void)
{
  volatile int xxx = 1;
  while(xxx);
}

#define PCLK1_MHZ 36
void i2c_clock_setup(void)
{
  uint32_t v = 0;
  reg32_clear_bit(I2C_CR1, I2C_CR1_PE);
   /*
    * PCLK1 = 36MHz (see rcc.c)
    */
  u32_modify_bits(&v, I2C_CR2_FREQ, I2C_CR2_FREQ_WIDTH, PCLK1_MHZ);
  reg_write(I2C_CR2, v);
  /*
   * T_low is time when clock signal was at logical 0 in a single clock pulse
   * T_high is time when clock signal was at logical 1 in a single clock pulse
   * T - is a time of one complete pulse
   * T = T_low + T_high
   * PCLK1 - is clock frequency of APB1, in our case its 36MHz 36000000
   * T_PCLK1 - is one period of APB1 clock = 1 / PCLK1
   *                                       = 1.0/36000000
   *                                       = 28 nanoseconds
   * In Standard mode:
   *   T_high = CCR * T_PCLK1
   *   T_low  = CCR * T_PCLK1
   *   ,so T_high = T_low
   * Desired I2C clock speed is 100KHz = 100000 
   * T (1 period) will be 1/100000 which is 1/100 of a millisecond,
   * same as 10 microseconds which is 10 000 nanoseconds
   * Because T_high = T_low and T = T_high + T_low, this results that
   * T_low = T / 2 = 10000 / 2 = 5000 nanoseconds T_high and T_low is required
   * Knowing T_PCLK1 and any of T_high or T_low we can calculate CCR
   * CCR = T_low / T_PCLK1 = 5000 / 28 = 178.57142857142858 = 179 (or 0x4f)
   *
   */
  u32_modify_bits(&v, I2C_CCR_CCR, I2C_CCR_CCR_WIDTH, 0x4f);
  reg_write(I2C_CCR, v);

  /*
   *T_RISE value in register is calculated as Maximum rize time in
   * Standard mode specification 1000ns / T_PCLK1 + 1
   * or PCLK1 / 1000000 + 1
   * in our case PCLK1 is 36 MHz so TRISE would be + 1 of that = 37
   */
  reg_write(I2C_TRISE, PCLK1_MHZ + 1);
  // v = (I2C_CCR_FS_FM << I2C_CCR_FS);
  reg32_set_bit(I2C_CR1, I2C_CR1_PE);
}

void i2c_start(void)
{
  reg32_set_bit(I2C_CR1, I2C_CR1_ACK);
  reg32_set_bit(I2C_CR1, I2C_CR1_START);
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_SB));
}

void i2c_write_addr(uint8_t addr)
{
  reg_write(I2C_DR, addr);
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_ADDR));
  volatile uint32_t v = reg_read(I2C_SR1) | reg_read(I2C_SR2);
}

void i2c_write_addr_mode_recv(uint8_t addr)
{
}

void i2c_write_byte(uint8_t v)
{
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_TXE));
  reg_write(I2C_DR, v);
}

void i2c_wait_stop(void)
{
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_BTF));
}

void i2c_write_bytes1(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
  i2c_start();
  i2c_write_addr(i2c_addr);
  i2c_write_byte(reg_addr);
  i2c_write_byte(data);
  i2c_wait_stop();
}

void i2c_read_bytes1(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data)
{
  /* Start / ACK */
  reg32_set_bit(I2C_CR1, I2C_CR1_ACK);
  reg32_set_bit(I2C_CR1, I2C_CR1_START);
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_SB));
  reg_write(I2C_DR, i2c_addr | 1);
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_ADDR));
  reg32_clear_bit(I2C_CR1, I2C_CR1_ACK);
  volatile uint32_t v = reg_read(I2C_SR1) | reg_read(I2C_SR2);
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_RXNE));
  *data = reg_read(I2C_DR);
  reg32_set_bit(I2C_CR1, I2C_CR1_STOP);
}

void i2c_write_bytes2(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1)
{
  i2c_start();
  i2c_write_addr(i2c_addr);
  i2c_write_byte(reg_addr);
  i2c_write_byte(data0);
  i2c_write_byte(data1);
  i2c_wait_stop();
}

void i2c_write_bytes(uint8_t i2c_addr, uint8_t *data, int count)
{
  i2c_start();
  i2c_write_addr(i2c_addr);
  for (int i = 0; i < count; ++i) {
    i2c_write_byte(data[i]);
  }
  i2c_wait_stop();
}

void i2c_write_bytes3(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1, uint8_t data2)
{
  i2c_start();
  i2c_write_addr(i2c_addr);
  i2c_write_byte(reg_addr);
  i2c_write_byte(data0);
  i2c_write_byte(data1);
  i2c_write_byte(data2);
  i2c_wait_stop();
}
