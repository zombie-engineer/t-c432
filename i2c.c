#include "i2c.h"
#include "i2c_regs.h"
#include "reg_access.h"

#define PCLK1_MHZ 36

// #define I2C_STANDARD_MODE
#define I2C_FAST_MODE

#if defined(I2C_FAST_MODE)
void i2c_clock_setup_fast(void)
{
  uint32_t v = 0;

  u32_modify_bits(&v, I2C_CCR_FS, 1, I2C_CCR_FS_FM);
  /*
   * In Fast mode:
   *   T_high = CCR * T_PCLK1
   *   T_low  = 2 * CCR * T_PCLK1
   *   CCR = T_high / T_PCLK1 => T_low = 2 * T_high
   *   ,so T_high = 2 * T_low
   *   T = T_low + T_high = 2 * T_high + T_high = 3 * T_high
   * Desired I2C clock speed is 400KHz = 400000
   * T (1 period) will be 1/400000 which is 1/400 of a millisecond,
   * same as 2.5 microseconds which is 2500 nanoseconds
   * Because T_high = 2 * T_low and T = T_high + T_low, this results that
   * T_high = T/3 = 2500 / 3 = 833.33 nanoseconds T_high and T_low is required
   * Knowing T_PCLK1 and any of T_high or T_low we can calculate CCR
   * CCR = T_high / T_PCLK1 = 833.33 / 28 = 29.76 = ~30 = 0x1e
   */
  u32_modify_bits(&v, I2C_CCR_CCR, I2C_CCR_CCR_WIDTH, 0x29);
  reg_write(I2C_CCR, v);
  reg_write(I2C_TRISE, PCLK1_MHZ + 1);
}
#endif

#if defined(I2C_STANDARD_MODE)
void i2c_clock_setup_standard(void)
{
  uint32_t v = 0;
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
}
#endif

void i2c_clock_setup(void)
{
  reg32_clear_bit(I2C_CR1, I2C_CR1_PE);

  uint32_t v = 0;
   /*
    * PCLK1 = 36MHz (see rcc.c)
    */
  u32_modify_bits(&v, I2C_CR2_FREQ, I2C_CR2_FREQ_WIDTH, PCLK1_MHZ);
  reg_write(I2C_CR2, v);

#if defined(I2C_STANDARD_MODE)
  i2c_clock_setup_standard();
#elif defined(I2C_FAST_MODE)
  i2c_clock_setup_fast();
#endif
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
  reg32_set_bit(I2C_CR1, I2C_CR1_STOP);
}

void i2c_write_bytes1(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
  i2c_start();
  i2c_write_addr(i2c_addr);
  i2c_write_byte(reg_addr);
  i2c_write_byte(data);
  i2c_wait_stop();
}

void i2c_write_bytes_x(uint8_t i2c_addr, uint8_t cmdbyte, uint8_t *data, int count)
{
  i2c_start();
  i2c_write_addr(i2c_addr);
  i2c_write_byte(cmdbyte);
  for (int i = 0; i < count; ++i) {
    i2c_write_byte(data[i]);
  }
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
