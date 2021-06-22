CROSS_COMPILE := /opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi-
PREFIX=/opt/trimui-toolchain/arm-buildroot-linux-gnueabi/sysroot/usr

TARGET=msettings

.PHONY: build
.PHONY: clean

CC = $(CROSS_COMPILE)gcc

SYSROOT     := $(shell $(CC) --print-sysroot)

INCLUDEDIR = $(SYSROOT)/usr/include
CFLAGS = -I$(INCLUDEDIR)
LDFLAGS = -s -lSDL -ltinyalsa -lrt -lmsettings -ldl

OPTM=-Ofast

build: 
	$(CC) -c -Werror -fpic "$(TARGET).c" -ltinyalsa
	$(CC) -shared -o "lib$(TARGET).so" "$(TARGET).o"
	cp "$(TARGET).h" "$(PREFIX)/include"
	cp "lib$(TARGET).so" "$(PREFIX)/lib"
	$(CC) -o "Settings.pak/$(TARGET)_client" client.c $(CFLAGS) $(LDFLAGS) $(OPTM)
	$(CC) -o "Settings.pak/$(TARGET)_host" host.c $(CFLAGS) $(LDFLAGS) $(OPTM)
clean:
	rm -f *.o
	rm -f "lib$(TARGET).so"
	rm -f "Settings.pak/$(TARGET)_client"
	rm -f "Settings.pak/$(TARGET)_host"
	rm -f $(PREFIX)/include/$(TARGET).h
	rm -f $(PREFIX)/lib/lib$(TARGET).so