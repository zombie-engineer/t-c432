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

define show_nvic
  printf "NVIC_ISERx(set-end): %08x\n", *(int *)0xe000e100
  printf "NVIC_ICERx(clr-ena): %08x\n", *(int *)0xe000e180
  printf "NVIC_ISPRx(set-pnd): %08x\n", *(int *)0xe000e200
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

reup
# b pushbutton_signal
c
show_i2c
show_nvic
show_usbr
