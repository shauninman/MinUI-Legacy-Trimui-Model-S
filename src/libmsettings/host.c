#include <stdio.h>
#include <stdlib.h>
#include <msettings.h>
#include <signal.h>
#include <string.h>

#include <stdint.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

// NOTE: simulating keymon

enum {
	TRIMUI_UP 		= KEY_UP,
	TRIMUI_DOWN 	= KEY_DOWN,
	TRIMUI_LEFT 	= KEY_LEFT,
	TRIMUI_RIGHT 	= KEY_RIGHT,
	TRIMUI_A 		= KEY_SPACE,
	TRIMUI_B 		= KEY_LEFTCTRL,
	TRIMUI_X 		= KEY_LEFTSHIFT,
	TRIMUI_Y 		= KEY_LEFTALT,
	TRIMUI_START 	= KEY_ENTER,
	TRIMUI_SELECT 	= KEY_RIGHTCTRL,
	TRIMUI_L 		= KEY_TAB,
	TRIMUI_R 		= KEY_BACKSPACE,
	TRIMUI_MENU 	= KEY_ESC,
};

volatile sig_atomic_t quit = 0;
void term(int signum)
{
   QuitSettings();
   quit = 1;
}


//	Global Variables
int	input_fd = 0;
struct input_event ev;
int	memdev = 0;
uint32_t *mem;

inline void openInputDevice(void) {
	FILE *fp;
	char strbuf[40];
	const char gpioname[] = "gpio_keys";
	for (uint32_t i=0; i<10; i++) {
		snprintf(strbuf,sizeof(strbuf),"/sys/class/input/event%d/device/name",i);
		fp = fopen(strbuf, "r");
		if ( fp != NULL ) {
			fgets(strbuf,sizeof(gpioname),fp);
			fclose(fp);
			if (!strcmp(strbuf,gpioname)) {
				snprintf(strbuf,sizeof(strbuf),"/dev/input/event%d",i);
				input_fd = open(strbuf, O_RDONLY);
				if (input_fd>0) return;
			}
		}
	}
}

#define RELEASED	0
#define PRESSED		1
#define REPEAT		2

#define VOLMAX		20
#define BRIMAX		10

void main(void) {
	struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
		
	InitSettings();
	openInputDevice();
	
	int v;
	while(!quit && read(input_fd, &ev, sizeof(ev)) == sizeof(ev) ) {
		if (ev.value==PRESSED) {
			switch (ev.code) {
				case TRIMUI_UP:
				v = GetBrightness();
				if (v<BRIMAX) SetBrightness(++v);
				break;
			
				case TRIMUI_DOWN:
				v = GetBrightness();
				if (v>0) SetBrightness(--v);
				break;
			
				case TRIMUI_X:
				v = GetVolume();
				if (v<VOLMAX) SetVolume(++v);
				break;
			
				case TRIMUI_B:
				v = GetVolume();
				if (v>0) SetVolume(--v);
				break;
			}
		}
	}
}