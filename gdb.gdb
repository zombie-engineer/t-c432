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
  printf "usb registers: "
  x/3wx 0x40005c40
end
