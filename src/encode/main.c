#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define	ENCODE	0
#define	DECODE	1

int main(int argc , char* argv[]) {
	unsigned char	input[256];
	unsigned char	output[256];
	unsigned char	serial[5];
	int		i;

if (argc != 2) {
	puts("Usage: encode filename");
} else {
	memset(input,0,256);
	memset(output,0,256);
	strncpy(input,argv[1],256);

	void* handle1 = dlopen("/usr/trimui/lib/libarelink.so", RTLD_LAZY);

	void* (*I2CSetInitial)() = dlsym(handle1, "I2CSetInitial");
	(*I2CSetInitial)();

	void* (*arelink_init)(char*) = dlsym(handle1, "arelink_init");
	(*arelink_init)(serial);
	printf("serial : %02X%02X%02X%02X%02X\n",serial[4],serial[3],serial[2],serial[1],serial[0]);

	void* (*arelink_req_enc_dec)(char*,char*,int) = dlsym(handle1, "arelink_req_enc_dec");

	for (i=0; i < 256; i=i+16) { (*arelink_req_enc_dec)(input+i,output+i,ENCODE); }

	dlclose(handle1);

	FILE* fp = fopen("/tmp/.tmenc","wb");
	fwrite(output,1,256,fp);
	fclose(fp);

	printf("encoded: %s\n",input);

}
	return 0;
}