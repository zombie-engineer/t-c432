set pagination off
target remote :3333
displ/8i $pc
x/8i $pc

define reup
  delete
  monitor program /home/user_user/stm32project/firmware.bin 0x08000000 verify
  monitor reset init
  add-symbol-file firmware.elf
end

define nvic_print_iser
  if $arg0 & (1 << 0)
    printf " WWDG"
  end
  if $arg0 & (1 << 1)
    printf " PVD"
  end
  if $arg0 & (1 << 2)
    printf " TAMPER"
  end
  if $arg0 & (1 << 3)
    printf " RTC"
  end
  if $arg0 & (1 << 4)
    printf " FLASH"
  end
  if $arg0 & (1 << 5)
    printf " RCC"
  end
  if $arg0 & (1 << 6)
    printf " EXTI0"
  end
  if $arg0 & (1 << 7)
    printf " EXTI1"
  end
  if $arg0 & (1 << 8)
    printf " EXTI2"
  end
  if $arg0 & (1 << 9)
    printf " EXTI3"
  end
  if $arg0 & (1 << 10)
    printf " EXTI4"
  end
  if $arg0 & (1 << 11)
    printf " DMA1_CHAN_1"
  end
  if $arg0 & (1 << 12)
    printf " DMA1_CHAN_2"
  end
  if $arg0 & (1 << 13)
    printf " DMA1_CHAN_3"
  end
  if $arg0 & (1 << 14)
    printf " DMA1_CHAN_4"
  end
  if $arg0 & (1 << 15)
    printf " DMA1_CHAN_5"
  end
  if $arg0 & (1 << 16)
    printf " DMA1_CHAN_6"
  end
  if $arg0 & (1 << 17)
    printf " DMA1_CHAN_7"
  end
  if $arg0 & (1 << 18)
    printf " ADC1"
  end
  if $arg0 & (1 << 19)
    printf " USB_HP_CAN_TX"
  end
  if $arg0 & (1 << 20)
    printf " USB_LP_CAN_RX0"
  end
  if $arg0 & (1 << 21)
    printf " CAN_RX1"
  end
  if $arg0 & (1 << 22)
    printf " CAN_SCE"
  end
  if $arg0 & (1 << 23)
    printf " EXTI9_5"
  end
  if $arg0 & (1 << 24)
    printf " TIM1_BRK"
  end
  if $arg0 & (1 << 25)
    printf " TIM1_UP"
  end
  if $arg0 & (1 << 26)
    printf " TIM1_TRG_COM"
  end
  if $arg0 & (1 << 27)
    printf " TIM1_CC"
  end
  if $arg0 & (1 << 28)
    printf " TIM2"
  end
  if $arg0 & (1 << 29)
    printf " TIM3"
  end
  if $arg0 & (1 << 30)
    printf " TIM4"
  end
  if $arg0 & (1 << 31)
    printf " I2C1_EV"
  end
  if $arg0 & (1 << 32)
    printf " I2C1_ER"
  end
  if $arg0 & (1 << 33)
    printf " I2C2_EV"
  end
  if $arg0 & (1 << 34)
    printf " I2C2_ER"
  end
  if $arg0 & (1 << 35)
    printf " SPI1"
  end
  if $arg0 & (1 << 36)
    printf " SPI2"
  end
  if $arg0 & (1 << 37)
    printf " USART1"
  end
  if $arg0 & (1 << 38)
    printf " USART2"
  end
  if $arg0 & (1 << 39)
    printf " USART3"
  end
  if $arg0 & (1 << 40)
    printf " EXTI10_15"
  end
  if $arg0 & (1 << 41)
    printf " RTC_ALARM"
  end
  if $arg0 & (1 << 42)
    printf " USB_WAKEUP"
  end
  if $arg0 & (1 << 43)
    printf " TIM8_BRK"
  end
  if $arg0 & (1 << 44)
    printf " TIM8_UP"
  end
  if $arg0 & (1 << 45)
    printf " TIM8_TRG_COM"
  end
  if $arg0 & (1 << 46)
    printf " TIM8_CC"
  end
  if $arg0 & (1 << 47)
    printf " ADC3"
  end
  if $arg0 & (1 << 48)
    printf " FSMC"
  end
  if $arg0 & (1 << 49)
    printf " SDIO"
  end
  if $arg0 & (1 << 50)
    printf " TIM5"
  end
  if $arg0 & (1 << 51)
    printf " SPI3"
  end
  if $arg0 & (1 << 52)
    printf " UART4"
  end
  if $arg0 & (1 << 53)
    printf " UART5"
  end
  if $arg0 & (1 << 54)
    printf " TIM6"
  end
  if $arg0 & (1 << 55)
    printf " TIM7"
  end
  if $arg0 & (1 << 56)
    printf " DMA2_CHAN_1"
  end
  if $arg0 & (1 << 57)
    printf " DMA2_CHAN_2"
  end
  if $arg0 & (1 << 58)
    printf " DMA2_CHAN_3"
  end
  if $arg0 & (1 << 59)
    printf " DMA2_CHAN_4_5"
  end
end

define show_nvic
  set $iser = *(int *)0xe000e100
  set $ispr = *(int *)0xe000e200

  printf "NVIC_ISERx(set-end): %08x", $iser
  nvic_print_iser $iser
  printf "\n"

  printf "NVIC_ICERx(clr-ena): %08x\n", *(int *)0xe000e180

  printf "NVIC_ISPRx(set-pnd): %08x", $ispr
  nvic_print_iser $ispr
  printf "\n"
  printf "NVIC_ICPRx(clr-pnd): %08x\n", *(int *)0xe000e280
  printf "NVIC_IABR0(active ): %08x\n", *(int *)0xe000e300
end

define epxr_decode
  if $arg0 & (1<<15)
    printf " OUT"
  end
  if $arg0 & (1<<11)
    printf " SETUP"
  end
  if $arg0 & (1<<7)
    printf " IN"
  end

  if (($arg0 >> 4) & 3) == 0
    printf " TX-DIS"
  end
  if (($arg0 >> 4) & 3) == 1
    printf " TX-STALL"
  end
  if (($arg0 >> 4) & 3) == 2
    printf " TX-NAK"
  end
  if (($arg0 >> 4) & 3) == 3
    printf " TX-VALID"
  end

  if (($arg0 >> 12) & 3) == 0
    printf " RX-DIS"
  end
  if (($arg0 >> 12) & 3) == 1
    printf " RX-STALL"
  end
  if (($arg0 >> 12) & 3) == 2
    printf " RX-NAK"
  end
  if (($arg0 >> 12) & 3) == 3
    printf " RX-VALID"
  end

  if $arg0 & (1<<6)
    printf " TX-DTOG"
  end

  if $arg0 & (1<<14)
    printf " RX-DTOG"
  end
end

define show_usbr
  printf "USB_CNTR: "
  x/1wx 0x40005c40
  set $istr = *(int *)0x40005c44
  printf "USB_ISTR: %08x", $istr
  if $istr & (1<<8)
    printf " ESOF"
  end
  if $istr & (1<<9)
    printf " SOF"
  end
  if $istr & (1<<10)
    printf " RST"
  end
  if $istr & (1<<11)
    printf " SUSP"
  end
  if $istr & (1<<12)
    printf " WKUP"
  end
  if $istr & (1<<13)
    printf " ERR"
  end
  if $istr & (1<<14)
    printf " PMAOVR"
  end
  if $istr & (1<<15)
    printf " CTR"
  end

  set $epxr = *(int *)0x40005c00
  printf "\nUSB_EP0R: %08x:", $epxr
  epxr_decode $epxr
  printf "\n"
  set $epxr = *(int *)0x40005c04
  printf "USB_EP1R: %08x:", $epxr
  epxr_decode $epxr
  printf "\n"
  set $epxr = *(int *)0x40005c08
  printf "USB_EP2R: %08x:", $epxr
  epxr_decode $epxr
  printf "\n"
  set $fnr = *(int *)0x40005c48
  printf "USB_FNR : %08x", $fnr
  printf "\n"

  set $fnr = *(int *)0x40005c4c
  printf "USB_DADDR: %08x", $fnr
  printf "\n"
end

define show_i2c
  set $cr1 = *(int *)0x40005400
  printf "I2C1_CR1: %08x", $cr1
  if $cr1 & (1<<0)
    printf " PE"
  end
  if $cr1 & (1<<8)
    printf " START"
  end
  if $cr1 & (1<<9)
    printf " STOP"
  end
  if $cr1 & (1<<10)
    printf " ACK"
  end
  if $cr1 & (1<<15)
    printf " SWRST"
  end

  set $sr1 = *(int *)0x40005414
  printf "\nI2C1_SR1: %08x", $sr1
  if $sr1 & (1<<0)
    printf " SB"
  end
  if $sr1 & (1<<1)
    printf " ADDR"
  end
  if $sr1 & (1<<2)
    printf " BTF"
  end
  if $sr1 & (1<<6)
    printf " RxNE"
  end
  if $sr1 & (1<<7)
    printf " TxE"
  end
  if $sr1 & (1<<8)
    printf " BERR"
  end
  if $sr1 & (1<<9)
    printf " ARLO"
  end
  if $sr1 & (1<<10)
    printf " AF"
  end
  if $sr1 & (1<<11)
    printf " OVR"
  end

  set $sr2 = *(int *)0x40005418
  printf "\nI2C1_SR2: %08x", $sr2
  if $sr2 & (1<<0)
    printf " MSL"
  end
  if $sr2 & (1<<1)
    printf " BUSY"
  end
  if $sr1 & (1<<2)
    printf " TR"
  end
  printf "\n"
end

define print_dma_channel
  set $dmanum = $arg0
  set $chnum = $arg1
  set $ccr = *(int *)(0x40020000 + $chnum * 0x14 + 8)
  set $cntr = *(int *)(0x40020000 + $chnum * 0x14 + 0x0c)
  set $pr = *(int *)(0x40020000 + $chnum * 0x14 + 0x10)
  set $mr = *(int *)(0x40020000 + $chnum * 0x14 + 0x14)
  printf "DMA%d.%d CCR:%04x, CNT:%04x,%08x", $dmanum + 1, $chnum + 1, $ccr, $cntr, $pr
  if ($ccr & (1<<6))
    printf "++"
  end
  if ($ccr & (1<<4))
    printf "<-"
  else
    printf "->"
  end
  printf "%08x", $mr
  if ($ccr & (1<<7))
    printf "++"
  end
  if ($ccr & (1<<0))
    printf "(ena)"
  else
    printf "(disa)"
  end
  printf "\n"
end

define show_dma
  set $isr = *(int *)0x40020000
  printf "DMA_ISR %08x\n", $isr
  print_dma_channel 0 0
  print_dma_channel 0 1
  print_dma_channel 0 2
  print_dma_channel 0 3
  print_dma_channel 0 4
  print_dma_channel 0 5
  print_dma_channel 0 6
end

reup

#  tb dbuf_flush
#    commands
#    b dma_transfer_setup
#    b dma_tx_cb
#  end
#  
#  b ssd1306_init


# tb task_wait_ms
#   commands
#     tb svc_handler
#     commands
#     tb scheduler_tick
#   end
#   c
# end

c

show_i2c
show_nvic
show_usbr

