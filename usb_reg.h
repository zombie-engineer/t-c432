#pragma once
#include "memory_layout.h"

#define USB_NUM_ENDPOINTS 8
#define USB_EP0R (volatile uint32_t *)(USB_BASE + 0x0000)

static inline volatile uint32_t *usb_get_ep_reg(int ep)
{
  return USB_EP0R + ep;
}

#define USB_EPXR_EA 0
#define USB_EPXR_EA_WIDTH 4
#define USB_EPXR_STAT_TX 4
#define USB_EPXR_STAT_TX_WIDTH 2
#define USB_EPXR_STAT_TX_DISABLED 0b00
#define USB_EPXR_STAT_TX_STALL    0b01
#define USB_EPXR_STAT_TX_NAK      0b10
#define USB_EPXR_STAT_TX_VALID    0b11

#define USB_EPXR_DTOG_TX 6
#define USB_EPXR_DTOG_TX_WIDTH 1
#define USB_EPXR_CTR_TX 7
#define USB_EPXR_CTR_TX_WIDTH 1
#define USB_EPXR_EP_KIND 8
#define USB_EPXR_EP_KIND_WIDTH 1
#define USB_EPXR_EP_TYPE 9
#define USB_EPXR_EP_TYPE_WIDTH 2
#define USB_EPXR_EP_TYPE_BULK 0b00
#define USB_EPXR_EP_TYPE_CONTROL 0b01
#define USB_EPXR_EP_TYPE_ISO 0b10
#define USB_EPXR_EP_TYPE_INTERRUPT 0b11

#define USB_EPXR_SETUP 11
#define USB_EPXR_SETUP_WIDTH 1
#define USB_EPXR_STAT_RX 12
#define USB_EPXR_STAT_RX_WIDTH 2
#define USB_EPXR_STAT_RX_DISABLED 0b00
#define USB_EPXR_STAT_RX_STALL    0b01
#define USB_EPXR_STAT_RX_NAK      0b10
#define USB_EPXR_STAT_RX_VALID    0b11

#define USB_EPXR_DTOG_RX 14
#define USB_EPXR_DTOG_RX_WIDTH 1
#define USB_EPXR_CTR_RX 15
#define USB_EPXR_CTR_RX_WIDTH 1

#define USB_EP1R (volatile uint32_t *)(USB_BASE + 0x0004)
#define USB_EP2R (volatile uint32_t *)(USB_BASE + 0x0008)
#define USB_EP3R (volatile uint32_t *)(USB_BASE + 0x000c)
#define USB_EP4R (volatile uint32_t *)(USB_BASE + 0x0010)
#define USB_EP5R (volatile uint32_t *)(USB_BASE + 0x0014)
#define USB_EP6R (volatile uint32_t *)(USB_BASE + 0x0018)
#define USB_EP7R (volatile uint32_t *)(USB_BASE + 0x001c)

#define USB_CNTR (volatile uint32_t *)(USB_BASE + 0x0040)
#define USB_ISTR (volatile uint32_t *)(USB_BASE + 0x0044)
#define USB_FNR (volatile uint32_t *)(USB_BASE + 0x0048)
#define USB_FNR_FN 0
#define USB_FNR_FN_WIDTH 11
#define USB_FNR_LSOF 11
#define USB_FNR_LSOF_WIDTH 2
#define USB_FNR_LCK 13
#define USB_FNR_LCK_WIDTH 1
#define USB_FNR_RXDM 14
#define USB_FNR_RXDM_WIDTH 1
#define USB_FNR_RXDP 15
#define USB_FNR_RXDP_WIDTH 1
#define USB_DADDR (volatile uint32_t *)(USB_BASE + 0x004c)
#define USB_DADDR_ADDR 0
#define USB_DADDR_ADDR_WIDTH 7
#define USB_DADDR_EF 7

#define USB_BTABLE (volatile uint32_t *)(USB_BASE + 0x0050)

#define USB_CNTR_FRES 0
#define USB_CNTR_PDWN 1
#define USB_CNTR_LPMODE 2
#define USB_CNTR_FSUSP 3
#define USB_CNTR_RESUME 4
#define USB_CNTR_ESOFM 8
#define USB_CNTR_SOFM 9
#define USB_CNTR_RESETM 10
#define USB_CNTR_SUSPM 11
#define USB_CNTR_WKUPM 12
#define USB_CNTR_ERRM 13
#define USB_CNTR_PMAOVRNM 14
#define USB_CNTR_CTRM 15

#define USB_ISTR_EP_ID 0
#define USB_ISTR_EP_ID_WIDTH 4
#define USB_ISTR_DIR 4
#define USB_ISTR_DIR_WIDTH 1
#define USB_ISTR_ESOF 8
#define USB_ISTR_SOF 9
#define USB_ISTR_RESET 10
#define USB_ISTR_SUSP 11
#define USB_ISTR_WKUP 12
#define USB_ISTR_ERR 13
#define USB_ISTR_PMAOVRN 14
#define USB_ISTR_CTR 15

#define USB_RAM (volatile uint32_t *)0x40006000
#define USB_COUNTn_RX_BLSIZE 15
#define USB_COUNTn_RX_BLSIZE_WIDTH 1
#define USB_COUNTn_RX_BLSIZE_2_BYTES 0
#define USB_COUNTn_RX_BLSIZE_32_BYTES 1
#define USB_COUNTn_RX_NUM_BLOCKS 10
#define USB_COUNTn_RX_NUM_BLOCK_WIDTH 5
#define USB_COUNTn_RX_BLSIZE_32_BYTES 1

#define USB_MAX_PACKET_SIZE 64

/* 2 blocks by 32 bytes = 64 bytes */
#define USB_COUNTn_RX_MAX_PACKET_SZ_64 \
  (USB_COUNTn_RX_BLSIZE_32_BYTES << USB_COUNTn_RX_BLSIZE) \
  | ((2 - 1) << USB_COUNTn_RX_NUM_BLOCKS)
#define PMA_TO_SRAM_ADDR(__pma_addr) (USB_RAM + (__pma_addr) / 2)
#define SRAM_TO_PMA_ADDR(__sram_addr) ((uint32_t)(__sram_addr) - (uint32_t)(USB_RAM) / 2)
