#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef volatile uint8_t *reg8_t;
typedef volatile uint16_t *reg16_t;
typedef volatile uint32_t *reg32_t;

#define reg_write8(__reg, __value) \
  *(reg8_t)(__reg) = (__value)

#define reg_write16(__reg, __value) \
  *(reg16_t)(__reg) = (__value)

#define reg_write(__reg, __value) \
  *(reg32_t)(__reg) = (__value)

#define reg_read(__reg) \
  (*(reg32_t)(__reg))

#define reg_read8(__reg) \
  (*(reg8_t)(__reg))

#define reg_read16(__reg) \
  (*(reg16_t)(__reg))

#define bitmask_size(__bitmask) (sizeof(__bitmask) * 8)

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

static inline bool u8_bit_is_set(uint8_t v, int bitpos)
{
  return v & (1 << bitpos);
}

static inline bool u16_bit_is_set(uint16_t v, int bitpos)
{
  return v & (1 << bitpos);
}

static inline bool u32_bit_is_set(uint32_t v, int bitpos)
{
  return v & (1 << bitpos);
}

static uint32_t u32_extract_bits(uint32_t v, int bitpos, int bitwidth)
{
  return (v >> bitpos) & u32_bitmask(bitwidth);
}

static inline void u16_set_bit(uint16_t *v, int bitpos)
{
  *v |= (1<<bitpos);
}

static inline void u32_set_bit(uint32_t *v, int bitpos)
{
  *v |= (1<<bitpos);
}

static inline void u32_clear_bit(uint32_t *v, int bitpos)
{
  *v &= ~(1<<bitpos);
}

static inline void reg16_modify_bits(reg16_t r, int bitpos, int bitwidth, int value)
{
  uint16_t tmp_v = reg_read16(r);
  uint16_t bitmask = ((uint16_t)((1 << bitwidth) - 1)) << bitpos;
  tmp_v &= ~bitmask;
  tmp_v |= (value << bitpos) & bitmask;
  reg_write(r, tmp_v);
}

static inline void reg32_modify_bits(reg32_t r, int bitpos, int bitwidth, int value)
{
  uint32_t tmp_v = reg_read(r);
  uint32_t bitmask = ((uint32_t)((1 << bitwidth) - 1)) << bitpos;
  tmp_v &= ~bitmask;
  tmp_v |= (value << bitpos) & bitmask;
  reg_write(r, tmp_v);
}

static inline void reg32_modify_bit(reg32_t r, int bitpos, int set)
{
  uint32_t set_bit = (set & 1) << bitpos;
  uint32_t clear_bit = ~(uint32_t)(1 << bitpos);
  reg_write(r, (reg_read(r) & clear_bit) | set_bit);
}

static inline void reg16_set_bit(reg16_t r, int bitpos)
{
  reg_write16(r, reg_read16(r) | (1<<bitpos));
}

static inline void reg32_set_bit(reg32_t r, int bitpos)
{
  reg_write(r, reg_read(r) | (1<<bitpos));
}

static inline void reg32_write_clear_bit(reg32_t r, int bitpos)
{
  reg_write(r, 1 << bitpos);
}

static inline void reg16_clear_bit(reg16_t r, int bitpos)
{
  reg_write16(r, reg_read16(r) & ~(1<<bitpos));
}

static inline void reg32_clear_bit(reg32_t r, int bitpos)
{
  reg_write(r, reg_read(r) & ~(1<<bitpos));
}

static inline bool reg16_bit_is_set(reg16_t r, int bitpos)
{
  return *r & (1 << bitpos);
}

static inline bool reg32_bit_is_set(reg32_t r, int bitpos)
{
  return *r & (1 << bitpos);
}

static inline bool reg32_bits_eq(reg32_t r, int bitpos, int bitwidth, int value)
{
  uint32_t v;
  uint32_t mask;

  v = reg_read(r);
  v >>= bitpos;
  mask = (1 << bitpos) -1;

  return (v & mask) == value;
}
