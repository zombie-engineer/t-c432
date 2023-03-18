#include "usb_driver.h"
#include "usb.h"
#include "usb_reg.h"
#include "nvic.h"
#include "reg_access.h"
#include "rcc.h"
#include "compiler.h"
#include "common_util.h"
#include "time.h"
#include "systick.h"
#include <svc.h>
#include <string.h>

#define USB_CONFIG_VALUE 1
#define USB_STRING_ID_VENDOR 1
#define USB_STRING_ID_PRODUCT 2
#define USB_STRING_ID_SERIAL 3

#define EP1_IN_MAX_PACKET_SIZE 64
#define EP1_IN_ADDR 1

#define EP2_OUT_ADDR 2
#define EP2_OUT_MAX_PACKET_SIZE 64
const struct usb_descriptor_device device_desc = {
  .bLength = sizeof(struct usb_descriptor_device),
  .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
  .bcdUSB = 0x0200,
  .bDeviceClass = 0xff,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = 64,
  .idVendor = 0xd001,
  .idProduct = 0xd002,
  .bcdDevice = 0x0200,
  .iManufacturer = USB_STRING_ID_VENDOR,
  .iProduct = USB_STRING_ID_PRODUCT,
  .iSerialNumber = USB_STRING_ID_SERIAL,
  .bNumConfigurations = 1
};

const struct usb_descriptor_device_qualifier device_qual = {
  .bLength = sizeof(struct usb_descriptor_device_qualifier),
  .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
  .bcdUSB = 0x0200,
  .bDeviceClass = 0xff,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = 64,
  .bNumConfigurations = 1,
  .bReserved = 0
};

struct usb_config_full {
  struct usb_descriptor_config c;
  struct usb_descriptor_interface i;
  struct usb_descriptor_endpoint ep1;
  struct usb_descriptor_endpoint ep2;
};

const struct usb_config_full config_desc = {
  .c = {
    .bLength = sizeof(struct usb_descriptor_config),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
    .wTotalLength = sizeof(struct usb_config_full),
    .bNumInterfaces = 1,
    .bConfigurationValue = USB_CONFIG_VALUE,
    .iConfiguration = 0,
    .bmAttributes = 0x80, /* bit 7 is reserved as 1 */
    .bMaxPower = 50 /* max power = 100mA */
  },
  .i = {
    .bLength = sizeof(struct usb_descriptor_interface),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternativeSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = 0xff, //2, /* CDC */
    .bInterfaceSubClass = 0, // 2, /* CDC ACM */
    .bInterfaceProtocol = 0, //1, /* Common AT commands */
    .iInterface = 0
  },
  .ep1 = {
    .bLength = sizeof(struct usb_descriptor_endpoint),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x80 | EP1_IN_ADDR,
    .bmAttributes = USB_EP_ATTRIBUTE_TYPE_BULK,
    .wMaxPacketSize = EP1_IN_MAX_PACKET_SIZE,
    .bInterval = 0
  },
  .ep2 = {
    .bLength = sizeof(struct usb_descriptor_endpoint),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = EP2_OUT_ADDR,
    .bmAttributes = USB_EP_ATTRIBUTE_TYPE_BULK,
    .wMaxPacketSize = EP2_OUT_MAX_PACKET_SIZE,
    .bInterval = 0
  }
};

const struct usb_descriptor_string_0 string_desc_0 = {
  .bLength = sizeof(struct usb_descriptor_string_0),
  .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  .wLangId = 0x0409
};

struct usb_descriptor_string_vendor {
  struct usb_descriptor_string_header h;
  char buf[14 * 2];
};

const struct usb_descriptor_string_vendor vendor_string_desc = {
  .h = {
    .bLength = sizeof(struct usb_descriptor_string_vendor),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  },
  .buf = {
    'C', 0,
    '4', 0,
    'a', 0,
    '0', 0,
    '$', 0,
    '-', 0,
    'E', 0,
    'n', 0,
    '9', 0,
    '1', 0,
    'N', 0,
    'E', 0,
    'E', 0,
    'r', 0,
  }
};

struct usb_descriptor_string_product {
  struct usb_descriptor_string_header h;
  char buf[13 * 2];
};

const struct usb_descriptor_string_product product_string_desc = {
  .h = {
    .bLength = sizeof(struct usb_descriptor_string_product),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  },
  .buf = {
    'r', 0,
    'o', 0,
    't', 0,
    't', 0,
    'e', 0,
    'd', 0,
    '.', 0,
    'O', 0,
    's', 0,
    'C', 0,
    '1', 0,
    'l', 0,
    'l', 0,
  }
};

struct usb_descriptor_string_serial {
  struct usb_descriptor_string_header h;
  char buf[11 * 2];
};

const struct usb_descriptor_string_serial serial_string_desc = {
  .h = {
    .bLength = sizeof(struct usb_descriptor_string_serial),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  },
  .buf = {
    '0', 0,
    'x', 0,
    'd', 0,
    '0', 0,
    '0', 0,
    '1', 0,
    '0', 0,
    'x', 0,
    'd', 0,
    '0', 0,
    '0', 0,
  }
};

static usb_driver_cb usb_driver_set_address_callback = NULL;
static usb_driver_cb usb_driver_tx_callback = NULL;
static usb_driver_cb usb_driver_rx_callback = NULL;

void usb_driver_set_cb(usb_driver_cb_t cb_type, usb_driver_cb cb)
{
  if (cb_type == USB_CB_SET_ADDRESS)
    usb_driver_set_address_callback = cb;
  else if (cb_type == USB_CB_TX)
    usb_driver_tx_callback = cb;
  else if (cb_type == USB_CB_RX)
    usb_driver_rx_callback = cb;
}

static void pmacpy_from(void *dst, uint32_t pma_offset, int num_bytes)
{
  const volatile uint32_t *from = PMA_TO_SRAM_ADDR(pma_offset);
  /* We are reading 4 bytes, extract only lower 2-byte part of it */
  int i;
  for (i = 0; i < num_bytes / 2; i++) {
    ((uint16_t *)dst)[i] = from[i] & 0xffff;
  }
}

static void pmacpy_to(uint32_t pma_dest, const void *src, uint16_t num_bytes)
{
  int i;
  volatile uint32_t *to = PMA_TO_SRAM_ADDR(pma_dest);
  const uint16_t *from = (const uint16_t *)src;
  for (i = 0; i < num_bytes / 2; i++) {
    to[i] = (uint32_t)from[i];
  }
}

struct ep_buf_desc {
  uint16_t tx_addr;
  uint16_t tx_count;
  uint16_t rx_addr;
  uint16_t rx_count;
};

struct usb_interrupt_stats {
  int num_sofs;
  int num_esofs;
  int num_errs;
  int num_susp;
  int num_wkup;
  int num_resets;
    /* 0:GET_DESCRIPTOR */
    /* 1:GET_DESCRIPTOR_IN_COMPLETED */
    /* 2:RESET */
    /* 3:SET_ADDRESS */
    /* 4:SET_ADDRESS_IN */
    /* 5:GET_DESCRIPTOR */
  int num_transactions;
};

static struct usb_interrupt_stats usbstats = { 0 };

void __usb_hp_isr(void)
{
  while(1);
}

static void usb_sof_handler(void)
{
  usbstats.num_sofs++;
}

static void usb_esof_handler(void)
{
  usbstats.num_esofs++;
}

static void usb_reset_clear_ram(void)
{
  volatile uint32_t *p = USB_RAM;
  for (int i = 0; i < 256; ++i)
  {
    *p++ = 0;
  }
}

/*
 * Initialize buffer description table
 * 0x40006000 (00) ADDR0_TX
 * 0x40006004 (02) COUNT0_TX
 * 0x40006008 (04) ADDR0_RX
 * 0x4000600c (06) COUNT0_RX

 * 0x40006010 (08) ADDR1_TX
 * 0x40006014 (0a) COUNT1_TX
 * 0x40006018 (0c) ADDR1_RX
 * 0x4000601c (0e) COUNT1_RX

 * 0x40006020 (10) ADDR2_TX
 * 0x40006024 (12) COUNT2_TX
 * 0x40006028 (14) ADDR2_RX
 * 0x4000602c (16) COUNT2_RX
 * ......
 * 0x40006020 (38) ADDR7_TX
 * 0x40006024 (3a) COUNT7_TX
 * 0x40006028 (3c) ADDR7_RX
 * 0x4000602c (40) COUNT7_RX
 * Table describes 8 endpoints.
 * Each endpoint has 8 bytes of data:
 * tx addr(2bytes), tx count(2bytes), rx addr(2bytes), rx count(2 bytes)
 * so Buffer descriptor table size is 8 * 8 = 64
 * Start of free area is after these first 64 bytes (0x40)
 */

/*
 * During store op to EPxR register bits CTR_TX and CTR_RX
 * when set to 1 are ignored and maintain their value.
 * Not initializing these bits to 1 will initialize them to 0
 * Writing 0 is same as clearing (and thus missing) one of those
 * two events
 */
#define USB_EPxR_STATIC_BITS(__ep, __type, __kind) \
  ( 0 \
    | ((__type) << USB_EPXR_EP_TYPE) \
    | ((__kind) << USB_EPXR_EP_KIND) \
    | (1 << USB_EPXR_CTR_TX) \
    | (1 << USB_EPXR_CTR_RX) \
    | (__ep & 0xf))

static void usb_reset_handler(void)
{
  struct ep_buf_desc ebdt[USB_NUM_ENDPOINTS] = { 0 };
  usbstats.num_resets++;
#if 0
  if (reg_read(USB_DADDR) & 0x7f)
    BRK;
#endif

  usb_reset_clear_ram();

  ebdt[0].tx_addr = sizeof(ebdt);
  ebdt[0].tx_count = 0;
  ebdt[0].rx_addr = sizeof(ebdt) + 64;
  ebdt[0].rx_count = USB_COUNTn_RX_MAX_PACKET_SZ_64;

  ebdt[1].tx_addr = sizeof(ebdt) + 64 * 2;
  ebdt[1].tx_count = 0;
  ebdt[1].rx_addr = 0;
  ebdt[1].rx_count = 0;

  ebdt[2].tx_addr = 0;
  ebdt[2].tx_count = 0;
  ebdt[2].rx_addr = sizeof(ebdt) + 64 * 3;
  ebdt[2].rx_count = USB_COUNTn_RX_MAX_PACKET_SZ_64;

  pmacpy_to(reg_read(USB_BTABLE), ebdt, sizeof(ebdt));

  reg_write(USB_BTABLE, 0);

  reg_write(usb_get_ep_reg(0), 0
    | USB_EPXR_EP_TYPE_CONTROL << USB_EPXR_EP_TYPE
    | 1                        << USB_EPXR_EP_KIND
    | USB_EPXR_STAT_TX_NAK     << USB_EPXR_STAT_TX
    | USB_EPXR_STAT_RX_VALID   << USB_EPXR_STAT_RX
  );

  reg_write(usb_get_ep_reg(1), 1
    | USB_EPXR_EP_TYPE_BULK    << USB_EPXR_EP_TYPE
    | USB_EPXR_STAT_TX_NAK     << USB_EPXR_STAT_TX
  );

  reg_write(usb_get_ep_reg(2), (2
    | USB_EPXR_EP_TYPE_BULK    << USB_EPXR_EP_TYPE
    | USB_EPXR_STAT_RX_VALID   << USB_EPXR_STAT_RX)
  );

  /* Set address 0 and enable */
  reg_write(USB_DADDR, 1 << USB_DADDR_EF);
}

static void usb_susp_handler(void)
{
  // reg32_set_bit(USB_CNTR, USB_CNTR_FSUSP);
  usbstats.num_susp++;
}

static void usb_wakeup_handler(void)
{
  usbstats.num_wkup++;
  // reg32_set_bit(USB_CNTR, USB_CNTR_RESUME);
}

static void bp(void)
{
}

static void usb_err_handler(void)
{
#if 0
  if (usbstats.num_errs)
    BRK;
#endif
  usbstats.num_errs++;
}

#define BTABLE_EP_FIELD_OFFSET(__ep, __field_off) ((__ep) * 16 + __field_off * 2)

#define BTABLE_FIELD_OFFSET_TX_ADDR(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 0)

#define BTABLE_FIELD_OFFSET_TX_COUNT(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 2)

#define BTABLE_FIELD_OFFSET_RX_ADDR(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 4)

#define BTABLE_FIELD_OFFSET_RX_COUNT(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 6)

static volatile uint32_t *pma_get_io_addr(int ep, int field_offset)
{
  volatile char *addr = ((volatile char *)USB_RAM) + reg_read(USB_BTABLE);

  return (volatile uint32_t *)(addr
    + BTABLE_EP_FIELD_OFFSET(ep, field_offset));
}

static uint16_t usb_pma_get_tx_addr(int ep)
{
  return reg_read(pma_get_io_addr(ep, 0)) & 0xffff;
}

static void usb_pma_set_tx_count(int ep, uint16_t value)
{
  reg_write(pma_get_io_addr(ep, 2), value);
}

static uint16_t usb_pma_get_tx_count(int ep)
{
  return reg_read(pma_get_io_addr(ep, 2)) & 0xffff;
}

static uint16_t usb_pma_get_rx_addr(int ep)
{
  return reg_read(pma_get_io_addr(ep, 4)) & 0xffff;
}

static uint16_t usb_pma_get_rx_count(int ep)
{
  return reg_read(pma_get_io_addr(ep, 6)) & u32_bitmask(10);
}

static uint16_t usb_pma_reset_rx_count(int ep)
{
  uint32_t v = reg_read(pma_get_io_addr(ep, 6)) & ~u32_bitmask(10);
  return reg_write(pma_get_io_addr(ep, 6), v);
}

struct usb_ctr_handler_log_entry {
  uint32_t dir;
  uint32_t is_setup;
  uint32_t epxr_on_entry;
  uint32_t epxr_on_exit;
};

static struct usb_ctr_handler_log_entry log[32] = { 0 };
static int log_idx = 0;

static uint32_t last_ep0r;

uint16_t addr = 0;

static uint32_t usb_get_static_epxr_bits(int ep)
{
  int type;
  int kind;

  if (ep == 0) {
    type = USB_EPXR_EP_TYPE_CONTROL;
    kind = 1;
  } else {
    type = USB_EPXR_EP_TYPE_BULK;
    kind = 0;
  }

  return USB_EPxR_STATIC_BITS(ep, type, kind);
}

/*
 * NAK   bits are 0b10 aka 2
 * VALID bits are 0b11 aka 3
 * to toggle from NAK to VALID from 2 to 3 we need XOR op
 * 2^3 = 0b10 XOR 0b11 = 0b01
 * Check - 2 TOGGLE 1 = 0b10 TOGGLE 0b01 = 0b11
 */
#define EP_TOGGLE_BITS_NAK_TO_VALID (2^3)

static void usb_ep_tx_nak_to_valid(int ep)
{
  uint32_t v = usb_get_static_epxr_bits(ep);
  v |= EP_TOGGLE_BITS_NAK_TO_VALID << 4;
  v |= addr;
  reg_write(usb_get_ep_reg(ep), v);
}

static void usb_ep_rx_nak_to_valid(int ep)
{
  uint32_t v = usb_get_static_epxr_bits(ep);
  v |= EP_TOGGLE_BITS_NAK_TO_VALID << 12;
  v |= addr;
  reg_write(usb_get_ep_reg(ep), v);
}

static void usb_ep_tx_nak_to_stall(int ep)
{
  uint32_t v = usb_get_static_epxr_bits(ep);
  reg_write(usb_get_ep_reg(ep), v | ((2 ^ 1)<<4));
}

static void usb_ep_set_status_out(int ep)
{
  uint32_t v = usb_get_static_epxr_bits(ep);
  reg_write(usb_get_ep_reg(ep), v | (1<<8));
}

static void usb_ep_rx_nak_to_stall(int ep)
{
  uint32_t v = usb_get_static_epxr_bits(ep);
  reg_write(usb_get_ep_reg(ep), v | ((2 ^ 1)<<12));
}

static void usb_handle_set_address(uint16_t address)
{
  uint32_t v = 0;
  u32_modify_bits(&v, USB_DADDR_ADDR, USB_DADDR_ADDR_WIDTH, address);
  u32_set_bit(&v, USB_DADDR_EF);
  reg_write(USB_DADDR, v);
  if (usb_driver_set_address_callback)
    usb_driver_set_address_callback(NULL);
}

static void usb_handle_get_descriptor(int ep, const struct usb_request *r)
{
  int type = (r->wValue >> 8) & 0xff;
  int index = r->wValue & 0xff;
  /*
   * num_transaction = 0 GET_DESCRIPTOR / DEVICE
   * num_transaction = 5 GET_DESCRIPTOR / DEVICE
   */
  if (type == USB_DESCRIPTOR_TYPE_DEVICE) {
    uint16_t tx_count = min(r->wLength, sizeof(device_desc));
    pmacpy_to(usb_pma_get_tx_addr(ep), &device_desc, tx_count);
    usb_pma_set_tx_count(ep, tx_count);
    usb_ep_tx_nak_to_valid(ep);
  } else if (type == USB_DESCRIPTOR_TYPE_CONFIGURATION) {
    uint16_t tx_count = min(r->wLength, sizeof(config_desc));
    pmacpy_to(usb_pma_get_tx_addr(ep), &config_desc, tx_count);
    usb_pma_set_tx_count(ep, tx_count);
    usb_ep_tx_nak_to_valid(ep);
  } else if (type == USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER) {
    uint16_t tx_count = min(r->wLength, sizeof(device_qual));
    pmacpy_to(usb_pma_get_tx_addr(ep), &device_qual, tx_count);
    usb_pma_set_tx_count(ep, tx_count);
    usb_ep_tx_nak_to_valid(ep);
  } else if (type == USB_DESCRIPTOR_TYPE_STRING) {
    if (index == 0) {
      uint16_t tx_count = min(r->wLength, sizeof(string_desc_0));
      pmacpy_to(usb_pma_get_tx_addr(ep), &string_desc_0, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    } else if (index == USB_STRING_ID_VENDOR) {
      uint16_t tx_count = min(r->wLength, sizeof(vendor_string_desc));
      pmacpy_to(usb_pma_get_tx_addr(ep), &vendor_string_desc, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    } else if (index == USB_STRING_ID_PRODUCT) {
      uint16_t tx_count = min(r->wLength, sizeof(product_string_desc));
      pmacpy_to(usb_pma_get_tx_addr(ep), &product_string_desc, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    } else if (index == USB_STRING_ID_SERIAL) {
      uint16_t tx_count = min(r->wLength, sizeof(serial_string_desc));
      pmacpy_to(usb_pma_get_tx_addr(ep), &serial_string_desc, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    }
  } else {
    BRK;
  }
}

static int next_addr = 0;
static int next_config = 0;
static bool usb_config_initialized = 0;

static void usb_handle_host_to_device_request(int ep,
  const struct usb_request *r)
{
  if (r->bRequest == USB_SETUP_REQUEST_SET_ADDRESS) {
    usb_pma_set_tx_count(ep, 0);
    usb_ep_tx_nak_to_valid(ep);
    next_addr = r->wValue & 0xf;
  } else if (r->bRequest == USB_SETUP_REQUEST_SET_CONFIGURATION) {
    usb_pma_set_tx_count(ep, 0);
    usb_ep_tx_nak_to_valid(ep);
    next_config = r->wValue & 0xf;
  }
  else {
    BRK;
  }
}

static void usb_handle_device_to_host_request(int ep,
  const struct usb_request *r)
{
  if (r->bRequest == USB_SETUP_REQUEST_GET_DESCRIPTOR) {
    usb_handle_get_descriptor(ep, r);
  }
  else {
    BRK;
  }
}

static void usb_ep_clear_ctr_tx(int ep)
{
  uint32_t v = usb_get_static_epxr_bits(ep);
  u32_clear_bit(&v, USB_EPXR_CTR_TX);
  reg_write(usb_get_ep_reg(ep), v);
}

static void usb_ep_clear_ctr_rx(int ep)
{
  uint32_t v = usb_get_static_epxr_bits(ep);
  u32_clear_bit(&v, USB_EPXR_CTR_RX);
  reg_write(usb_get_ep_reg(ep), v);
}

static int usb_ep_read_rx_packet(int ep, char *buf, int maxsz)
{
  int num_bytes = usb_pma_get_rx_count(ep);
  if (num_bytes <= maxsz) {
    pmacpy_from(buf, usb_pma_get_rx_addr(ep), num_bytes);
  }

  return num_bytes;
}

int ddx = 0;

static void ep1_tx_handler()
{
  usb_ep_clear_ctr_tx(1);
  ddx++;
  char buf[EP1_IN_MAX_PACKET_SIZE];

  memset(buf, 'A' + ddx, sizeof(buf));
  *(uint32_t *)buf = ddx;

  pmacpy_to(usb_pma_get_tx_addr(EP1_IN_ADDR),
    buf, sizeof(buf));

  usb_pma_set_tx_count(EP1_IN_ADDR, sizeof(buf));
  reg_write(pma_get_io_addr(EP1_IN_ADDR, 6), 0);
  usb_ep_tx_nak_to_valid(EP1_IN_ADDR);

  if (usb_driver_tx_callback)
    usb_driver_tx_callback(buf);
}

extern uint8_t led_bitmask[6];

static void ep2_rx_handler()
{
  char buf[64];
  memset(buf, 0, sizeof(buf));

  pmacpy_from(buf,
    usb_pma_get_rx_addr(EP2_OUT_ADDR),
    usb_pma_get_rx_count(EP2_OUT_ADDR));
  usb_pma_reset_rx_count(EP2_OUT_ADDR);

  memcpy(led_bitmask, buf, sizeof(led_bitmask));
  usb_ep_rx_nak_to_valid(EP2_OUT_ADDR);
  usb_ep_clear_ctr_rx(EP2_OUT_ADDR);

#if 0
  if (usb_driver_rx_callback)
    usb_driver_rx_callback(buf);
#endif
}

static void usb_ep0_handler(int dir)
{
  struct usb_ctr_handler_log_entry *l = &log[log_idx++];
  l->epxr_on_entry = reg_read(usb_get_ep_reg(0));
  l->dir = dir;
  l->is_setup = u32_bit_is_set(l->epxr_on_entry, USB_EPXR_SETUP);

  if (dir) {
    /* CTR_RX should be set because this is an RX (OUT or SETUP) packet */
    if (!reg32_bit_is_set(usb_get_ep_reg(0), USB_EPXR_CTR_RX)) {
      BRK;
    }

    usb_ep_clear_ctr_rx(0);
    if (reg32_bit_is_set(usb_get_ep_reg(0), USB_EPXR_SETUP)) {

      struct usb_request r;
      if (usb_ep_read_rx_packet(0, (void *)&r, sizeof(r)) < sizeof(r)) {
        BRK;
      }
      usb_pma_reset_rx_count(0);

      if (r.bmRequestType == USB_REQUEST_DEVICE_TO_HOST_STANDARD) {
        usb_handle_device_to_host_request(0, &r);
      }
      else if (r.bmRequestType == USB_REQUEST_HOST_TO_DEVICE_STANDARD) {
        usb_handle_host_to_device_request(0, &r);
      }
      else {
        BRK;
      }
    }
  } else {
    if (next_addr) {
      usb_handle_set_address(next_addr);
      next_addr = 0;
    }
    if (next_config) {
      if (next_config == USB_CONFIG_VALUE) {
        ep1_tx_handler();
        usb_config_initialized = 1;
      }
      next_config = 0;
    }

    usb_ep_clear_ctr_tx(0);
    usb_pma_set_tx_count(0, 0);
    usb_ep_tx_nak_to_stall(0);
    usb_ep_rx_nak_to_valid(0);
  }
  usbstats.num_transactions++;
  l->epxr_on_exit = reg_read(usb_get_ep_reg(0));
}

static void usb_ctr_handler(int ep, int dir)
{
  if (ep == 0) {
    usb_ep0_handler(dir);
  }
  else if (ep == EP1_IN_ADDR) {
    ep1_tx_handler();
  }
  else if (ep == EP2_OUT_ADDR) {
    ep2_rx_handler();
  }
}

void __usb_lp_isr(void)
{
  uint32_t v;
  v = reg_read(USB_ISTR);
  if (u32_bit_is_set(v, USB_ISTR_ERR)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_ERR);
    usb_err_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_RESET)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_RESET);
    usb_reset_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_SOF)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_SOF);
    usb_sof_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_ESOF)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_ESOF);
    usb_esof_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_SUSP)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_SUSP);
    usb_susp_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_WKUP)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_WKUP);
    usb_wakeup_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_CTR)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_CTR);
    usb_ctr_handler(
      u32_extract_bits(v, USB_ISTR_EP_ID, USB_ISTR_EP_ID_WIDTH),
      u32_extract_bits(v, USB_ISTR_DIR, USB_ISTR_DIR_WIDTH));
  }
  else
  {
    reg_write(USB_ISTR, 0);
    bp();
    while(1);
  }
}

void __usb_wakeup_isr(void)
{
  // while(1);
}

void usb_init(void)
{
  svc_wait_ms(20);
  rcc_periph_ena(RCC_PERIPH_USB);
  rcc_periph_ena(RCC_PERIPH_IOPA);
  rcc_periph_ena(RCC_PERIPH_AFIO);
  // TODO As soon as the USB is enabled, these pins are automatically connected to
  // the USB internal transceiver.
#if 0
  gpioa_set_cr(11, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
  gpioa_set_cr(12, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
#endif
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0);
  reg_write(NVIC_ISER1, 1 << NVIC_INTERRUPT_NUMBER_USB_WAKEUP - 32);

  /* Enable interrupts on RESET request from host */
  reg32_set_bit(USB_CNTR, USB_CNTR_RESETM);
  /* Enable interrupts on ERR condition */
  reg32_set_bit(USB_CNTR, USB_CNTR_ERRM);

  /* Disable power-down bit, USB peripheral powers up */
  reg32_clear_bit(USB_CNTR, USB_CNTR_PDWN);

  svc_wait_ms(1);
  /* On power clear Force-RESET bit to exit reset state */
  reg32_clear_bit(USB_CNTR, USB_CNTR_FRES);
  /* Clear all possible spurious interrupts */
  reg_write(USB_ISTR, 0);
}
