FLAGS	= -Os -ltinyalsa -Wl,--gc-sections -s -DMINUI

all:
	/opt/trimui-toolchain/bin/arm-linux-gcc keymon.c -o keymon $(FLAGS)
clean:
	rm -rf keymon
