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

define nvic
  printf "nvic enabled: "
  x/3wx 0xe000e100
  printf "nvic pending: "
  x/3wx 0xe000e200
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

define usbr
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

reup

c
usbr
