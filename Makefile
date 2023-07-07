CROSSDIR := /home/user_user/Downloads/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi
LIBGCC := $(CROSSDIR)/lib/gcc/arm-none-eabi/12.2.0/thumb/v7-m/nofp/libgcc.a
LIBC := $(CROSSDIR)/arm-none-eabi/lib/thumb/v7-m/nofp/libc.a
$(info $(LIBC))
INCLUDES := -I.

DISPLAY_DRIVER := sh1106
# DIODE_DRIVER := ws2812b

#  ui/adc_widget.o 
OBJS := main.o \
  ui/main_widget.o \
  ui/usb_widget.o \
  ui/bar_widget.o \
  ui/text_widget.o \
  ui/value_widget.o \
  ui/navi_header_widget.o \
  ui/arrow_button_widget.o \
  display.o \
  fault.o \
  exti.o \
  pushbuttons.o \
  svc.o \
  spi.o \
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
  start.o \
  usb_driver.o \
  usart.o \
  systick.o

ifeq "$(DIODE_DRIVER)" "ws2812b"
  OBJS += drivers/ws2812b/ws2812b.o
  OBJS += ui/led_widget.o
endif

ifeq "$(DISPLAY_DRIVER)" "sh1106"
  OBJS += drivers/sh1106/sh1106.o
endif
ifeq "$(DISPLAY_DRIVER)" "ssd1306"
  OBJS += ssd1306.o
endif

$(info DISPLAY_DRIVER: $(DISPLAY_DRIVER))

firmware.bin: firmware.elf
	arm-none-eabi-objcopy firmware.elf --output-target binary firmware.bin

firmware.elf: $(OBJS) link.ld
	arm-none-eabi-ld $(OBJS) $(LIBGCC) $(LIBC) -o firmware.elf -T link.ld -Map firmware.map

%.o: %.S
	arm-none-eabi-as $< -o $@

%.o : %.c
	arm-none-eabi-gcc -c $(INCLUDES) -g $< -o $@ -mthumb -mcpu=cortex-m3

d:
	arm-gnueabihfgdb -q -x gdb.gdb firmware.elf
#	arm-none-eabi-gdb -q -x gdb.gdb firmware.elf

hd: firmware.bin
	hexdump -v -e '1/4 "%08_ax %08X \n"' $? | less

clean:
	rm -v *.o
