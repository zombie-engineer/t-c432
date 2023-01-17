#include "i2c.h"
#include "i2c_regs.h"
#include "reg_access.h"
#include "scheduler.h"
#include "nvic.h"
#include "svc.h"
#include "dma.h"

#define PCLK1_MHZ 36

// #define I2C_STANDARD_MODE
#define I2C_FAST_MODE

#define I2C_TRANSFER_DMA (1<<0)
/* i2c request */
struct i2c_rq {
  uint32_t size;
  const uint8_t *data;
  uint8_t addr;
  uint8_t small_data[5];
  int flags;
};

/* first only maintain 1 request */
struct i2c_rq current_i2c_rq;

static bool use_dma_transfer = false;
static int dma_ch_tx = 0;
static int dma_ch_rx = 0;

typedef enum {
  I2C_ASYNC_STATE_IDLE,
  I2C_ASYNC_STATE_WAIT_EV5,
  I2C_ASYNC_STATE_WAIT_EV6,
  I2C_ASYNC_STATE_WAIT_EV8_1,
  I2C_ASYNC_STATE_WAIT_EV8,
  I2C_ASYNC_STATE_WAIT_EV8_2
} i2c_async_state_t;

static i2c_async_state_t i2c_async_state = I2C_ASYNC_STATE_IDLE;
static uint32_t i2c_async_transfer_completed = false;

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

void i2c_write_async(uint8_t i2c_addr, const uint8_t *data, int count, bool dma)
{
  current_i2c_rq.size = count;
  current_i2c_rq.addr = i2c_addr;
  current_i2c_rq.data = data;
  if (dma) {
    current_i2c_rq.flags |= I2C_TRANSFER_DMA;
    reg32_set_bit(I2C_CR2, I2C_CR2_DMAEN);
    reg32_clear_bit(I2C_CR2, I2C_CR2_ITBUFEN);
  }
  else {
    current_i2c_rq.flags = 0;
    reg32_set_bit(I2C_CR2, I2C_CR2_ITBUFEN);
    reg32_clear_bit(I2C_CR2, I2C_CR2_DMAEN);
  }

  reg32_set_bit(I2C_CR1, I2C_CR1_ACK);
  reg32_set_bit(I2C_CR1, I2C_CR1_START);
  i2c_async_state = I2C_ASYNC_STATE_WAIT_EV5;

  while(i2c_async_state != I2C_ASYNC_STATE_IDLE) asm volatile("wfe");
#if 0
  {
    svc_wait_on_flag(&i2c_async_transfer_completed);
  }
  i2c_async_transfer_completed = false;
#endif
}

void i2c_write_sync(uint8_t i2c_addr, const uint8_t *data, int count)
{
  /* Start condition */
  reg32_set_bit(I2C_CR1, I2C_CR1_ACK);
  reg32_set_bit(I2C_CR1, I2C_CR1_START);
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_SB));

  /* Address */
  reg_write(I2C_DR, i2c_addr);
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_ADDR));
  volatile uint32_t v = reg_read(I2C_SR1) | reg_read(I2C_SR2);

  /* Bytes */
  for (int i = 0; i < count; ++i) {
    while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_TXE));
    reg_write(I2C_DR, v);
  }
  /* Stop condition */
  while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_BTF));
  reg32_set_bit(I2C_CR1, I2C_CR1_STOP);
}

static void dma_tx_cb(void)
{
  reg32_clear_bit(I2C_CR2, I2C_CR2_DMAEN);
  dma_transfer_disable(dma_ch_tx);
  if (i2c_async_state != I2C_ASYNC_STATE_WAIT_EV8_2)
    asm volatile ("bkpt");
}

void i2c_init_isr(bool use_dma)
{
  nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_I2C1_EV);
  nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_I2C1_ER);
  reg32_set_bit(I2C_CR2, I2C_CR2_ITERREN);
  reg32_set_bit(I2C_CR2, I2C_CR2_ITEVTEN);
  use_dma_transfer = use_dma;
  if (use_dma) {
    dma_init();
    dma_ch_tx = dma_get_channel_id(DMA_PERIPH_I2C1_TX);
    if (dma_ch_tx)
      dma_ch_tx -= 1;
    dma_enable_interrupt(DMA_NUM_1, dma_ch_tx);
    dma_set_isr_cb(DMA_NUM_1, dma_ch_tx, dma_tx_cb);

    dma_ch_rx = dma_get_channel_id(DMA_PERIPH_I2C1_RX);
    if (dma_ch_rx)
      dma_ch_rx -= 1;

  } else {
    reg32_set_bit(I2C_CR2, I2C_CR2_ITBUFEN);
  }
}

void i2c_isr_disable(void)
{
  nvic_disable_interrupt(NVIC_INTERRUPT_NUMBER_I2C1_EV);
  nvic_disable_interrupt(NVIC_INTERRUPT_NUMBER_I2C1_ER);
  reg32_clear_bit(I2C_CR2, I2C_CR2_ITERREN);
  reg32_clear_bit(I2C_CR2, I2C_CR2_ITEVTEN);
  reg32_clear_bit(I2C_CR2, I2C_CR2_ITBUFEN);
}

#define I2C_INTERRUPT_TYPE_EVENT 0
#define I2C_INTERRUPT_TYPE_ERROR 1

void i2c_handle_event(void)
{
  volatile uint32_t sr1 = reg_read(I2C_SR1);

  switch (i2c_async_state) {
    /*
     * SB=1 (Start bit). Start condition completed, now we need to send address
     * DMA:
     * According to RM0008 we need to setup DMA registers for future data
     * stage, that will happen right after address stage. After we write
     * address to the I2C_DR register, address will be transmitted and after
     * that TxE bit in I2C_SR1 register will be triggered. DMA requests are
     * mapped to rising TxE event.
     */
    case I2C_ASYNC_STATE_WAIT_EV5:
    {
      if (!u32_bit_is_set(sr1, I2C_SR1_SB))
        svc_call(SVC_PANIC);

      if (current_i2c_rq.flags & I2C_TRANSFER_DMA) {
        dma_transfer_setup(
          dma_ch_tx,
          I2C_DR,
          current_i2c_rq.data,
          current_i2c_rq.size);
      }

      reg_write(I2C_DR, current_i2c_rq.addr);
      i2c_async_state = I2C_ASYNC_STATE_WAIT_EV6;
      break;
    }
    /* ADDR=1, cleared by reading SR1, then SR2 */
    case I2C_ASYNC_STATE_WAIT_EV6:
    {
      if (!u32_bit_is_set(sr1, I2C_SR1_ADDR) ||
          !reg32_bit_is_set(I2C_SR2, I2C_SR2_BUSY))
        svc_call(SVC_PANIC);

      if (current_i2c_rq.flags & I2C_TRANSFER_DMA)
        i2c_async_state = I2C_ASYNC_STATE_WAIT_EV8_2;
      else
        i2c_async_state = I2C_ASYNC_STATE_WAIT_EV8_1;
      break;
    }
    /*
     * Address sent, we received ACK from slave, now we send out first data
     * byte.Now TXE should be enabled
     */
    case I2C_ASYNC_STATE_WAIT_EV8_1:
    {
      if (!u32_bit_is_set(sr1, I2C_SR1_TXE))
        svc_call(SVC_PANIC);

      if (current_i2c_rq.flags & I2C_TRANSFER_DMA) {
        asm volatile ("bkpt");
      } else {
        reg_write(I2C_DR, *current_i2c_rq.data);
        current_i2c_rq.data++;
        current_i2c_rq.size--;
        i2c_async_state = I2C_ASYNC_STATE_WAIT_EV8;
      }
      break;
    }
    case I2C_ASYNC_STATE_WAIT_EV8:
    {
      if (!u32_bit_is_set(sr1, I2C_SR1_TXE))
        svc_call(SVC_PANIC);

      if (current_i2c_rq.flags & I2C_TRANSFER_DMA) {
        asm volatile ("bkpt");
      } else {
        reg_write(I2C_DR, *current_i2c_rq.data);
        current_i2c_rq.size--;
        current_i2c_rq.data++;
      }

      if (current_i2c_rq.size > 0) {
        i2c_async_state = I2C_ASYNC_STATE_WAIT_EV8;
      } else {
        i2c_async_state = I2C_ASYNC_STATE_WAIT_EV8_2;
      }
      break;
    }
    case I2C_ASYNC_STATE_WAIT_EV8_2:
    {
      // asm volatile ("bkpt");
      if (!u32_bit_is_set(sr1, I2C_SR1_TXE)
        || !u32_bit_is_set(sr1, I2C_SR1_BTF))
      {
        // while(!reg32_bit_is_set(I2C_SR1, I2C_SR1_BTF));
//        svc_call(SVC_PANIC);
      }

      sr1 = reg_read(I2C_SR1);
      reg32_set_bit(I2C_CR1, I2C_CR1_STOP);
      /*
       * STOP condition will need some time removing TxE bit, which causes
       * interrupt event, which we don't want
       */
      nvic_clear_pending(NVIC_INTERRUPT_NUMBER_I2C1_EV);
      i2c_async_state = I2C_ASYNC_STATE_IDLE;
      // i2c_async_transfer_completed = true;
      // scheduler_signal_flag_irq(&i2c_async_transfer_completed);
      break;
    }
    default:
      // asm volatile ("bkpt");
      break;
  }
}

static void i2c_handle_ack_failure(void)
{
  asm volatile ("bkpt");
  switch (i2c_async_state) {
    case I2C_ASYNC_STATE_IDLE:
      break;
    case I2C_ASYNC_STATE_WAIT_EV5:
      break;
    case I2C_ASYNC_STATE_WAIT_EV6:
      break;
    case I2C_ASYNC_STATE_WAIT_EV8_1:
      break;
    case I2C_ASYNC_STATE_WAIT_EV8:
      break;
    case I2C_ASYNC_STATE_WAIT_EV8_2:
    default:
      asm volatile ("bkpt");
      break;
  }
}

void i2c_handle_error(void)
{
  uint32_t sr1 = reg_read(I2C_SR1);
  if (u32_bit_is_set(sr1, I2C_SR1_BERR)) {
    asm volatile ("bkpt");
  }
  if (u32_bit_is_set(sr1, I2C_SR1_ARLO)) {
    asm volatile ("bkpt");
  }
  if (u32_bit_is_set(sr1, I2C_SR1_AF)) {
    i2c_handle_ack_failure();
    u32_clear_bit(&sr1, I2C_SR1_AF);
  }
  if (u32_bit_is_set(sr1, I2C_SR1_OVR)) {
    asm volatile ("bkpt");
  }
  if (u32_bit_is_set(sr1, I2C_SR1_PECERR)) {
    asm volatile ("bkpt");
  }
  if (u32_bit_is_set(sr1, I2C_SR1_TIMEOUT)) {
    asm volatile ("bkpt");
  }
  if (u32_bit_is_set(sr1, I2C_SR1_SMBALERT)) {
    asm volatile ("bkpt");
  }

  asm volatile ("bkpt");
}

void i2c1_isr(int interrupt_type)
{
  if (interrupt_type == I2C_INTERRUPT_TYPE_EVENT)
    i2c_handle_event();
  else
    i2c_handle_error();
}
