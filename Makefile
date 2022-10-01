
firmware.bin: firmware.elf
	arm-none-eabi-objcopy firmware.elf --output-target binary firmware.bin

firmware.elf: main.o start.o isr_vector.o rcc.o i2c.o flash.o gpio.o link.ld
	arm-none-eabi-ld main.o start.o isr_vector.o rcc.o flash.o gpio.o i2c.o -o firmware.elf -T link.ld

start.o: start.S
	arm-none-eabi-as start.S -o start.o

main.o: main.c
	arm-none-eabi-gcc -c -g main.c -o main.o -mthumb

gpio.o: gpio.c
	arm-none-eabi-gcc -c -g gpio.c -o gpio.o -mthumb

i2c.o: i2c.c
	arm-none-eabi-gcc -c -g i2c.c -o i2c.o -mthumb

flash.o: flash.c
	arm-none-eabi-gcc -c -g flash.c -o flash.o -mthumb

rcc.o: rcc.c
	arm-none-eabi-gcc -c -g rcc.c -o rcc.o -mthumb

isr_vector.o: isr_vector.c
	arm-none-eabi-gcc -c isr_vector.c -o isr_vector.o

d:
	arm-none-eabi-gdb -q -x gdb.gdb firmware.elf

hd: firmware.bin
	hexdump -v -e '1/4 "%08_ax %08X \n"' $? | less
