CROSS_COMPILE := /opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi-
PREFIX=/opt/trimui-toolchain/arm-buildroot-linux-gnueabi/sysroot/usr

TARGET=mmenu

.PHONY: build
.PHONY: clean

CC = $(CROSS_COMPILE)gcc

SYSROOT     := $(shell $(CC) --print-sysroot)

INCLUDEDIR = $(SYSROOT)/usr/include
CFLAGS = -I$(INCLUDEDIR)
LDFLAGS = -s -lSDL -lSDL_image -lSDL_ttf -lz -lm

OPTM=-Ofast

build: 
	$(CC) -c -Wall -Werror -fpic "$(TARGET).c" $(CFLAGS) $(LDFLAGS) $(OPTM)
	$(CC) -shared -o "lib$(TARGET).so" "$(TARGET).o"
	cp "$(TARGET).h" "$(PREFIX)/include"
	cp "lib$(TARGET).so" "$(PREFIX)/lib"
clean:
	rm -f *.o
	rm -f "lib$(TARGET).so"
	rm -f $(PREFIX)/include/$(TARGET).h
	rm -f $(PREFIX)/lib/lib$(TARGET).so