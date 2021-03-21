CROSS_COMPILE := /opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi-

TARGET=confirm

.PHONY: build
.PHONY: clean

CC = $(CROSS_COMPILE)gcc

SYSROOT     := $(shell $(CC) --print-sysroot)

INCLUDEDIR = $(SYSROOT)/usr/include
CFLAGS = -I$(INCLUDEDIR)
LDFLAGS = -s -lSDL -lSDL_image -lz -lm

OPTM=-O3

build: 
	$(CC) -o $(TARGET) main.c $(CFLAGS) $(LDFLAGS) $(OPTM)
clean:
	rm -f $(TARGET)