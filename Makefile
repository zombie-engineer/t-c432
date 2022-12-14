CROSSDIR := /home/user_user/Downloads/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi
LIBGCC := $(CROSSDIR)/lib/gcc/arm-none-eabi/12.2.0/thumb/v7-m/nofp/libgcc.a
LIBC := $(CROSSDIR)/arm-none-eabi/lib/thumb/v7-m/nofp/libc.a
$(info $(LIBC))
INCLUDES := -I.

OBJS := main.o \
  ui/main_widget.o \
  ui/bar_widget.o \
  ui/arrow_button_widget.o \
  exti.o \
  pushbuttons.o \
  svc.o \
  scheduler.o \
  task.o \
  main_task.o \
  ui.o \
  adc.o \
  dma.o \
  scb.o \
  nvic.o \
  tim.o \
  time.o \
  font.o\
  isr_vector.o\
  rcc.o \
  i2c.o \
  flash.o \
  gpio.o \
  ssd1306.o \
  start.o \
  usb_driver.o \
  usart.o \
  systick.o

firmware.bin: firmware.elf
	arm-none-eabi-objcopy firmware.elf --output-target binary firmware.bin

firmware.elf: $(OBJS) link.ld
	arm-none-eabi-ld $(OBJS) $(LIBGCC) $(LIBC) -o firmware.elf -T link.ld -Map firmware.map

%.o: %.S
	arm-none-eabi-as $< -o $@

%.o : %.c
	arm-none-eabi-gcc -c $(INCLUDES) -g $< -o $@ -mthumb -mcpu=cortex-m3

d:
	arm-none-eabi-gdb -q -x gdb.gdb firmware.elf

hd: firmware.bin
	hexdump -v -e '1/4 "%08_ax %08X \n"' $? | less

clean:
	rm -v *.o
