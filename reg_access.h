#pragma once
#include <stdbool.h>
#include <stdint.h>

#define reg_write(__reg, __value) \
  *(volatile uint32_t *)(__reg) = (__value)

#define reg_write8(__reg, __value) \
  *(volatile uint8_t *)(__reg) = (__value)

#define reg_read(__reg) \
  (*(volatile uint32_t *)(__reg))

#define reg_read8(__reg) \
  (*(volatile uint8_t *)(__reg))

static inline void u32_modify_bits(uint32_t *v, int bitpos, int bitwidth, int value)
{
  uint32_t tmp_v = *v;
  uint32_t bitmask = ((uint32_t)((1 << bitwidth) - 1)) << bitpos;
  tmp_v &= ~bitmask;
  tmp_v |= (value << bitpos) & bitmask;
  *v = tmp_v;
}

static inline uint32_t u32_bitmask(int bitpos)
{
  return (1 << bitpos) - 1;
}

static inline bool u32_bit_is_set(uint32_t v, int bitpos)
{
  return v & (1 << bitpos);
}

static uint32_t u32_extract_bits(uint32_t v, int bitpos, int bitwidth)
{
  return (v >> bitpos) & u32_bitmask(bitwidth);
}

static inline void u32_set_bit(volatile uint32_t *v, int bitpos)
{
  *v |= (1<<bitpos);
}

static inline void u32_clear_bit(volatile uint32_t *v, int bitpos)
{
  *v &= ~(1<<bitpos);
}

static inline void reg32_modify_bits(volatile uint32_t *v, int bitpos, int bitwidth, int value)
{
  uint32_t tmp_v = reg_read(v);
  uint32_t bitmask = ((uint32_t)((1 << bitwidth) - 1)) << bitpos;
  tmp_v &= ~bitmask;
  tmp_v |= (value << bitpos) & bitmask;
  reg_write(v, tmp_v);
}

static inline void reg32_modify_bit(volatile uint32_t *v, int bitpos, int set)
{
  uint32_t set_bit = (set & 1) << bitpos;
  uint32_t clear_bit = ~(uint32_t)(1 << bitpos);
  reg_write(v, (reg_read(v) & clear_bit) | set_bit);
}

static inline void reg32_set_bit(volatile uint32_t *v, int bitpos)
{
  reg_write(v, reg_read(v) | (1<<bitpos));
}

static inline void reg32_clear_bit(volatile uint32_t *v, int bitpos)
{
  reg_write(v, reg_read(v) & ~(1<<bitpos));
}

static inline bool reg32_bit_is_set(volatile uint32_t *addr, int bitpos)
{
  uint32_t v = *(uint32_t *)addr;
  return v & (1 << bitpos);
}

static inline bool reg32_bits_eq(volatile uint32_t *addr, int bitpos, int bitwidth, int value)
{
  uint32_t v;
  uint32_t mask;

  v = reg_read(addr);
  v >>= bitpos;
  mask = (1 << bitpos) -1;

  return (v & mask) == value;
}


