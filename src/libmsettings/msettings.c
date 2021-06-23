#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <tinyalsa/asoundlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <string.h>

#include "msettings.h"

typedef struct Settings {
	int version; // future proofing
	int brightness;
	int headphones;
	int speaker;
	int unused[4]; // for future use
} Settings;
static Settings DefaultSettings = {
	.version = 1,
	.brightness = 5,
	.headphones = 4,
	.speaker = 8,
};
static Settings* settings;

#define SHM_KEY "/SharedSettings"
#define kSettingsPath "/mnt/settings.bin"
static int shm_fd = -1;
static int is_host = 0;
static int shm_size = sizeof(Settings);

#define SHOUT(msg) puts(msg); fflush(stdout)
#define HasUSBAudio() access("/dev/dsp1", R_OK | W_OK)==0

static void* libtinyalsa = NULL; // NOTE: required to remove cascading linking requirement
void InitSettings(void) {
	if (!libtinyalsa) libtinyalsa = dlopen("/usr/lib/libtinyalsa.so", RTLD_LAZY | RTLD_GLOBAL);
	
	puts("InitSettings()");
	
	shm_fd = shm_open(SHM_KEY, O_RDWR | O_CREAT | O_EXCL, 0644); // see if it exists
	if (shm_fd==-1 && errno==EEXIST) { // already exists
		puts("\tclient");
		shm_fd = shm_open(SHM_KEY, O_RDWR, 0644);
		settings = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	}
	else { // host
		puts("\thost");
		is_host = 1;
		// we created it so set initial size and populate
		ftruncate(shm_fd, shm_size);
		settings = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		
		int fd = open(kSettingsPath, O_RDONLY);
		if (fd>=0) {
			// load from file
			puts("load");
			read(fd, settings, shm_size);
			// TODO: use settings->version for future proofing
			close(fd);
		}
		else {
			// load defaults
			puts("default");
			memcpy(settings, &DefaultSettings, shm_size);
		}
		printf("\tbrightness: %i\n", settings->brightness);
		printf("\theadphones: %i\n", settings->headphones);
		printf("\tspeaker   : %i\n", settings->speaker);
	} 
	fflush(stdout);
}
void QuitSettings(void) {
	munmap(settings, shm_size);
	if (is_host) {
		puts("quit host");
		shm_unlink(SHM_KEY);
	}
	else {
		puts("quit client");
	}
	fflush(stdout);
}
static inline void SaveSettings(void) {
	int fd = open(kSettingsPath, O_CREAT|O_WRONLY, 0644);
	if (fd>=0) {
		write(fd, settings, shm_size);
		close(fd);
	}
}

int GetBrightness(void) {
	return settings->brightness;
}
int GetVolume(void) {
	return HasUSBAudio() ? settings->headphones : settings->speaker;
}

void SetBrightness(int value) {
	SetRawBrightness(70 + (value * 5)); // 0..10 -> 70..120
	printf("set brightness: %i\n", value);
	settings->brightness = value;
	SaveSettings();
}
void SetVolume(int value) {
	SetRawVolume(value * 5); // 0..20 -> 0..100 (%)
	printf("set volume: %i\n", value);
	if (HasUSBAudio()) {
		puts("\theadphones");
		settings->headphones = value;
	}
	else {
		puts("\tspeaker");
		settings->speaker = value;
	}
	
	SaveSettings();
}

void SetRawBrightness(int val) {
	int fd = open("/sys/class/disp/disp/attr/lcdbl", O_WRONLY);
	if (fd>=0) {
		dprintf(fd,"%d",val);
		close(fd);
	}
}
void SetRawVolume(int val) {
	struct mixer *mixer;
	struct mixer_ctl *ctl;
	if (HasUSBAudio()) {
		mixer = mixer_open(1);
		if (mixer != NULL) {
			// for USB headphones
			ctl = mixer_get_ctl(mixer,2);
			if (ctl) {
				mixer_ctl_set_percent(ctl,0,val);
				mixer_ctl_set_percent(ctl,1,val);
			}
			// for USB headset
			ctl = mixer_get_ctl(mixer,4);
			if (ctl) {
				mixer_ctl_set_percent(ctl,0,val);
				mixer_ctl_set_percent(ctl,1,val);
			}
			mixer_close(mixer);
			return;
		}
	}
	else {
		// for internal speaker
		mixer = mixer_open(0);
		if (mixer != NULL) {
			ctl = mixer_get_ctl(mixer,22);
			if (ctl) {
				mixer_ctl_set_percent(ctl,0,val);
			}
			mixer_close(mixer);
		}
	}
}