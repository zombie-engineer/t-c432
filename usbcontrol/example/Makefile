all: led_ui

led_ui:
	gcc main.c -g `pkg-config --libs --cflags gtk+-2.0` -lusb-1.0
