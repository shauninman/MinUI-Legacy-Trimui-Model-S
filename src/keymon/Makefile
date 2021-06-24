FLAGS	= -Os -lmsettings -ltinyalsa -lpthread -lrt -ldl -Wl,--gc-sections -s

all:
	/opt/trimui-toolchain/bin/arm-linux-gcc keymon.c -o keymon $(FLAGS)
clean:
	rm -rf keymon
