#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <tinyalsa/asoundlib.h>

//	Button Defines
#define	BUTTON_MENU	KEY_ESC
#define	BUTTON_SELECT	KEY_RIGHTCTRL
#define	BUTTON_START	KEY_ENTER
#define	BUTTON_L	KEY_TAB
#define	BUTTON_R	KEY_BACKSPACE

//	for keyshm
#define VOLUME		0
#define BRIGHTNESS	1
#define VOLMAX		20
#define BRIMAX		10

//	for ev.value
#define RELEASED	0
#define PRESSED		1
#define REPEAT		2

//	for button_flag
#define SELECT_BIT	0
#define START_BIT	1
#define SELECT		(1<<SELECT_BIT)
#define START		(1<<START_BIT)

#ifndef MINUI
//	for suspend (about 2sec)
#define SUSPEND_TIME	40
#endif

//	for DEBUG
//#define	DEBUG
#ifdef	DEBUG
#define ERROR(str)	fprintf(stderr,str"\n"); quit(EXIT_FAILURE)
#else
#define ERROR(str)	quit(EXIT_FAILURE)
#endif

//	for JSON
#define JSONKEYSMAX 6
#define JSONSTRSIZE 16
#define	JSON_VOLUME 0
#define	JSON_BRIGHTNESS 4

const char jsonname[] = "/mnt/system.json";
const char jsonkeyname[JSONKEYSMAX][JSONSTRSIZE] = {
	"\"vol\"","\"keymap\"","\"mute\"","\"bgmvol\"","\"brightness\"","\"language\""};
char	jsonvalue[JSONKEYSMAX][JSONSTRSIZE] = {
	"20","0","1","0","10","\"en.lang\""};

//	Global Variables
int			input_fd = 0;
struct input_event	ev;
int			memdev = 0;
uint32_t		*mem;

#ifndef MINUI
//	for Screenshot
#define			DEBE_LAY3_FB_ADDR_REG	0x85C
uint32_t		*debe_map;
uint32_t		fb_addr;
#endif

//
//	Quit
//
void quit(int exitcode) {
	if (input_fd > 0) close(input_fd);
	if (memdev > 0) close(memdev);
	exit(exitcode);
}

//
//	Trim Strings for reading json
//
char* trimstr(char* str, uint32_t first) {
	char *firstchar, *firstlastchar, *lastfirstchar, *lastchar;
	uint32_t i;

	firstchar = firstlastchar = lastfirstchar = lastchar = 0;

	for (i=0; i<strlen(str); i++) {
		if ((str[i]!='\r')&&(str[i]!='\n')&&(str[i]!=' ')&&(str[i]!='\t')&&
		    (str[i]!='{')&&(str[i]!='}')&&(str[i]!=',')) {
			if (!firstchar) {
				firstchar = &str[i];
				lastfirstchar = &str[i];
			}
			lastchar = &str[i];
			if (i) {
				if ((str[i-1]=='\r')||(str[i-1]=='\n')||(str[i-1]==' ')||(str[i-1]=='\t')||
				    (str[i-1]=='{')||(str[i-1]=='}')||(str[i-1]==',')) {
					lastfirstchar = &str[i];
				}
			}
		} else {
			if (!firstlastchar) {
				firstlastchar = lastchar;
			}
		}
	}
	if (first) {
		lastfirstchar = firstchar;
		lastchar = firstlastchar;
	}
	if (lastchar) {
		lastchar[1] = 0;
	}
	if (lastfirstchar) return lastfirstchar;
	return 0;
}

//
//	Read system.json
//
void readJson(void) {
	FILE *fp;
	char key[256];
	char val[256];
	char *keyptr, *valptr;
	int f;
	uint32_t i;

	fp = fopen(jsonname, "r");
	if (fp != NULL) {
		key[0] = 0; val[0] = 0;
		while ((f = fscanf(fp, "%255[^:]:%255[^\n]\n", key, val)) != EOF) {
			if (!f) { if (fscanf(fp,"%*[^\n]\n") == EOF) break; else continue; }
			if (!(keyptr = trimstr(key,0))) continue;
			if (!(valptr = trimstr(val,1))) { val[0] = 0; valptr = val; }
			for (i=0; i<JSONKEYSMAX; i++) {
				if (!strcmp(keyptr,&jsonkeyname[i][0])) {
					strncpy(&jsonvalue[i][0],valptr,JSONSTRSIZE-1);
					break;
				}
			}
			key[0] = 0; val[0] = 0;
		}
		fclose(fp);
	}
}

//
//	Write system.json
//
void writeJson(void) {
	FILE *fp;
	uint32_t i;

	fp = fopen(jsonname, "w");
	if (fp != NULL) {
		fputs("{\n\t",fp);
		for (i=0; i<JSONKEYSMAX; i++) {
			fputs(&jsonkeyname[i][0],fp);
			fputs(":\t",fp);
			fputs(&jsonvalue[i][0],fp);
			if (i != JSONKEYSMAX-1) {
				fputs(",\n\t",fp);
			} else {
				fputs("\n}",fp);
			}
		}
		fflush(fp);
		fsync(fileno(fp));
		fclose(fp);
	}
}

//
//	Get numeric value from json
//
inline uint32_t getValue(uint32_t key) {
	if (key < JSONKEYSMAX-1) {	// except language
		return (uint32_t)atoi(&jsonvalue[key][0]);
	} else return 0;
}

//
//	Set numeric value to json
//
inline void setValue(uint32_t key,uint32_t val) {
	if (key < JSONKEYSMAX-1) {	// except language
		snprintf(&jsonvalue[key][0],JSONSTRSIZE,"%d",val);
	}
}

//
//	Init LCD
//
inline void initLCD(void) {
	memdev = open("/dev/mem", O_RDWR);
	if (memdev > 0) {
		mem = (uint32_t*)mmap(0, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, memdev, 0x01c20000); // CCU/INTC/PIO/TIMER
		if (mem == MAP_FAILED) {
			close(memdev);
			ERROR("Failed to mmap hardware registers");
		} else {
			uint32_t pe_cfg0 = mem[0x0890>>2]; // PE_CFG0
			if (pe_cfg0 & 1) {
				mem[0x0890>>2] = pe_cfg0 & 0xF0FFFFFF | 0x03000000;
			}
#ifndef MINUI
			// Save FrameBuffer Start Address (for Screenshot)
			debe_map = (uint32_t*)mmap(0, 0x1000, PROT_READ, MAP_SHARED, memdev, 0x01e60000); // DEBE
			if (debe_map == MAP_FAILED) {
				close(memdev);
				ERROR("Failed to mmap hardware registers");
			}
			fb_addr = debe_map[DEBE_LAY3_FB_ADDR_REG>>2];
#endif
		}
	} else {
		ERROR("Failed to open /dev/mem");
	}
}

//
//	Open Input Device
//
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
	ERROR("Failed to open /dev/input/event");
}

//
//	Set Volume
//
void setVolume(uint32_t val) {
	struct mixer *mixer;
	struct mixer_ctl *ctl;

	setValue(JSON_VOLUME,val);

	// 0..20 -> 0..100 (%)
	val = val * 5;

	if (access("/dev/dsp1", R_OK | W_OK) == 0) {
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
	// for internal speaker
	mixer = mixer_open(0);
	if (mixer != NULL) {
		ctl = mixer_get_ctl(mixer,22);
		if (ctl) {
			mixer_ctl_set_percent(ctl,0,val);
		}
		mixer_close(mixer);
	} else {
		ERROR("Failed to open mixer");
	}
}

//
//	Set Brightness(Raw)
//
void setBrightnessRaw(uint32_t val) {
	int fd = open("/sys/class/disp/disp/attr/lcdbl", O_WRONLY);
	if (fd <= 0) {
		ERROR("Failed to open /sys/class/disp/disp/attr/lcdbl");
	}
	dprintf(fd,"%d",val);
	close(fd);
}

//
//	Set Brightness
//
void setBrightness(uint32_t val) {
	setValue(JSON_BRIGHTNESS,val);

	// 0..10 -> 70..120
	setBrightnessRaw(70 + (val * 5));
}

#ifndef MINUI
//
//	Screenshot
//
inline void screenshot(void) {
	const uint8_t bmpheaderv4[] = {		// 320x240 RGB565 BMPv4 Header (0x00 .. 0x3F)
		0x42,0x4D,0x7A,0x58,0x02,0x00,0x00,0x00,0x00,0x00,0x7A,0x00,0x00,0x00,0x6C,0x00,
		0x00,0x00,0x40,0x01,0x00,0x00,0xF0,0x00,0x00,0x00,0x01,0x00,0x10,0x00,0x03,0x00,
		0x00,0x00,0x00,0x58,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0xE0,0x07,0x00,0x00,0x1F,0x00 };
#define	bmpv4ofs	0x7a			// 0x40 .. 0x79 = 00
	const char	fb0name[] = "/dev/fb0";
	char		screenshotname[32];
	uint8_t		linebuffer[320*2];
	FILE		*fp_s;
	FILE		*fp_d;
	int		i;
	uint32_t	fb_offset;

	for (i=0; i<100; i++) {
		snprintf(screenshotname,sizeof(screenshotname),"/mnt/SDCARD/screenshot%02d.bmp",i);
		if ( access(screenshotname, R_OK) != 0 ) break;
	} if (i > 99) return;

	fp_s = fopen(fb0name, "rb");
	if (fp_s != NULL) {
		fp_d = fopen(screenshotname, "wb");
		if (fp_d != NULL) {
			fwrite(bmpheaderv4,sizeof(bmpheaderv4),1,fp_d);
			fseek(fp_d,bmpv4ofs,SEEK_SET);
			fb_offset = (debe_map[DEBE_LAY3_FB_ADDR_REG>>2] - fb_addr)>>3;
			for (i=239; i>=0; i--) {
				fseek(fp_s,i*320*2+fb_offset,SEEK_SET);
				fread(linebuffer,sizeof(linebuffer),1,fp_s);
				fwrite(linebuffer,sizeof(linebuffer),1,fp_d);
			}
			fclose(fp_d);
			sync();
		}
		fclose(fp_s);
	}
}

//
//	Suspend
//
inline void suspend(void) {
	DIR *procdp;
	struct dirent *dir;
	char fname[32];
	pid_t keymon_pid = getpid();
	pid_t parent_pid = getppid();
	pid_t pid;
	pid_t ppid;
	char state;
	char comm[128];
#define PIDMAX 32
	uint32_t suspendpid[PIDMAX];
	suspendpid[0] = 0;

	// Send SIGSTOP to active processes
	// Cond:1. PID is greater than keymon's
	// 	2. PPID is greater than or equal to keymon's
	//	3. state is "R" or "S" or "D"
	//	4. comm is not "(sh)" or "(ash)"
	procdp = opendir("/proc");
	while (dir = readdir(procdp)) {
		if (dir->d_type == DT_DIR) {
			pid = atoi(dir->d_name);
			if ( pid > keymon_pid) {
				sprintf(fname, "/proc/%d/stat", pid);
				FILE *fd = fopen(fname, "r");
				if (fd != NULL) {
					fscanf(fd, "%*d %128s %c %d", &comm, &state, &ppid);
					fclose(fd);
				} else { ERROR("Failed to open stat"); }
				if ((ppid >= parent_pid)&&((state == 'R')||(state == 'S')||(state == 'D'))) {
					if ((strcmp(comm,"(sh)"))&&(strcmp(comm,"(ash)"))) {
						if ( suspendpid[0] >= PIDMAX ) { ERROR("Too many processes"); }
						suspendpid[++suspendpid[0]] = pid;
						kill(pid,SIGSTOP);
					}
				}
			}
		}
	}
	closedir(procdp);

	// Set Brightness to 0
	setBrightnessRaw(0);

	// Flush Write Cache
	sync();

	// Wait for Release START button (1)
	do {	if ( read(input_fd, &ev, sizeof(ev)) != sizeof(ev) ) break;
	} while ((ev.type != EV_KEY) || (ev.code != BUTTON_START) || ( ev.value != RELEASED ));

	// Set CPU clock to 16MHz
	uint32_t cpuclock = mem[0];
	mem[0] = (cpuclock & 0xFFFF0000) | 0x0112;

	// Wait for Release START button (2)
	uint32_t screenshot_flag = 0;
	uint32_t terminateprocess_flag = 0;
	uint32_t killprocess_flag = 0;
	do {	if ( read(input_fd, &ev, sizeof(ev)) != sizeof(ev) ) break;
		// press SELECT while suspend ... Screenshot
		if ((ev.type == EV_KEY) && (ev.code == BUTTON_SELECT) && ( ev.value == PRESSED ) && (screenshot_flag == 0)) {
			mem[0] = cpuclock;
			setBrightnessRaw(128);
			screenshot();
			screenshot_flag = 1;
			// Screenshot Effect
			for (uint32_t i=128; i>0; i--) {
				setBrightnessRaw(i-1);
				usleep(5000);
			}
			mem[0] = (cpuclock & 0xFFFF0000) | 0x0112;
		}
		// hold L + release START ... Terminate Active Process
		// hold R + release START ... Kill Active Process
		if ((ev.type == EV_KEY) && (ev.code == BUTTON_L)) terminateprocess_flag = ev.value;
		if ((ev.type == EV_KEY) && (ev.code == BUTTON_R)) killprocess_flag = ev.value;
	} while ((ev.type != EV_KEY) || (ev.code != BUTTON_START) || ( ev.value != RELEASED ));

	// Restore CPU clock
	mem[0] = cpuclock;

	// Restore Brightness
	readJson();
	setBrightness(getValue(JSON_BRIGHTNESS));

	// Send SIGCONT to suspended processes
	if (suspendpid[0]) {
		for (uint32_t i=1; i <= suspendpid[0]; i++) {
			kill(suspendpid[i],SIGCONT);
			if (terminateprocess_flag) kill(suspendpid[i],SIGTERM);
			if (killprocess_flag) kill(suspendpid[i],SIGKILL);
		}
	}
}
#endif

//
//	Main
//
void main(void) {
	// Initialize
	signal(SIGTERM, &quit);
	signal(SIGSEGV, &quit);
	initLCD();
	openInputDevice();

	// Set Initial Volume / Brightness
	readJson();
	setVolume(getValue(JSON_VOLUME));
	setBrightness(getValue(JSON_BRIGHTNESS));

	// Main Loop
	register uint32_t val;
	register uint32_t pressedbuttons = 0;
	register uint32_t button_flag = 0;
	uint32_t repeat_START = 0; //	for suspend
	uint32_t repeat_LR = 0;
	while( read(input_fd, &ev, sizeof(ev)) == sizeof(ev) ) {
		val = ev.value;
		if (( ev.type != EV_KEY ) || ( val > REPEAT )) continue;
		if ( val < REPEAT ) {
			pressedbuttons += val;
			if (( val == RELEASED )&&( pressedbuttons > 0 )) pressedbuttons--;
		}
		switch (ev.code) {
		case BUTTON_SELECT:
			if ( val != REPEAT ) {
				button_flag = button_flag & (~SELECT) | (val<<SELECT_BIT);
			}
			break;
		case BUTTON_START:
			if ( val != REPEAT ) {
				button_flag = button_flag & (~START) | (val<<START_BIT);
				repeat_START = (pressedbuttons == 1 ? val : 0);
			} 
#ifndef MINUI
			else {
				if (repeat_START) {
					if (repeat_START++ > SUSPEND_TIME) {
						// Hold START only : suspend
						suspend();
						button_flag = repeat_START = pressedbuttons = 0;
					}
				}
			}
#endif
			break;
		case BUTTON_L:
			if ( val == REPEAT ) {
				// Adjust repeat speed to 1/2
				val = repeat_LR;
				repeat_LR ^= PRESSED;
			} else {
				repeat_LR = 0;
			}
			if ( val == PRESSED ) {
				switch (button_flag) {
				case SELECT:
					// SELECT + L : volume down
					readJson();
					val = getValue(JSON_VOLUME);
					if (val>0) {
						setVolume(--val);
						writeJson();
					}
					break;
				case START:
					// START + L : brightness down
					readJson();
					val = getValue(JSON_BRIGHTNESS);
					if (val>0) {
						setBrightness(--val);
						writeJson();
					}
					break;
				default:
					break;
				}
			}
			break;
		case BUTTON_R:
			if ( val == REPEAT ) {
				// Adjust repeat speed to 1/2
				val = repeat_LR;
				repeat_LR ^= PRESSED;
			} else {
				repeat_LR = 0;
			}
			if ( val == PRESSED ) {
				switch (button_flag) {
				case SELECT:
					// SELECT + R : volume up
					readJson();
					val = getValue(JSON_VOLUME);
					if (val<VOLMAX) {
						setVolume(++val);
						writeJson();
					}
					break;
				case START:
					// START + R : brightness up
					readJson();
					val = getValue(JSON_BRIGHTNESS);
					if (val<BRIMAX) {
						setBrightness(++val);
						writeJson();
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
	ERROR("Failed to read input event");
}
