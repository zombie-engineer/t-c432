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

define usregs
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
  printf "\nUSB_EP0R: "
  x/1wx 0x40005c00
end

# reup
