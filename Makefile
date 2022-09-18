
firmware.bin: firmware.elf
	arm-none-eabi-objcopy firmware.elf --output-target binary firmware.bin

firmware.elf: main.o start.o exception_table.o link.ld
	arm-none-eabi-ld main.o start.o exception_table.o -o firmware.elf -T link.ld

start.o: start.S
	arm-none-eabi-as start.S -o start.o

main.o: main.c
	arm-none-eabi-gcc -c main.c -o main.o

exception_table.o: exception_table.c
	arm-none-eabi-gcc -c exception_table.c -o exception_table.o

d:
	arm-none-eabi-gdb -x gdb.gdb firmware.elf


