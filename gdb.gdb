set pagination off
target remote :3333
displ/8i $pc
x/8i $pc

define reup
monitor program /home/user_user/stm32project/firmware.bin 0x08000000 verify
monitor reset init
end
