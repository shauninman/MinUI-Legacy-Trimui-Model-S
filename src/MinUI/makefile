CROSS_COMPILE := /opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi-

TARGET=MinUI

.PHONY: build
.PHONY: clean

CC = $(CROSS_COMPILE)gcc

SYSROOT     := $(shell $(CC) --print-sysroot)

INCLUDEDIR = $(SYSROOT)/usr/include
CFLAGS = -I$(INCLUDEDIR)
LDFLAGS = -s -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf -lz -lm

OPTM=-O3

build: 
	$(CC) -o $(TARGET) main.c $(CFLAGS) $(LDFLAGS) $(OPTM) -ldl -rdynamic
clean:
	rm -f $(TARGET)