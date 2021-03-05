PREFIX=/opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi-

TARGET=encode

.PHONY: build
.PHONY: clean
	
build:
	$(PREFIX)gcc main.c -o $(TARGET) -O3 -ldl -rdynamic
	$(PREFIX)strip $(TARGET)
clean:
	rm -f $(TARGET)
