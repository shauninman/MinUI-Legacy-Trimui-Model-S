#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <unistd.h>
#include <dlfcn.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

///////////////////////////////////////

#define kRootDir "/mnt/SDCARD"
#define kRecentlyPlayedDir kRootDir "/Recently Played"
#define kLastPath "/tmp/last.txt"

///////////////////////////////////////

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

static void error_handler(int sig) {
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	// fflush(stderr);
	// fclose(stderr);
	exit(1);
}

///////////////////////////////////////

static int match_prefix(char* pre, char* str) {
	return (strncmp(pre,str,strlen(pre))==0);
}
static int match_suffix(char* suf, char* str) {
	int len = strlen(suf);
	return (strncmp(suf, str+strlen(str)-len, len)==0);
}
static int exact_match(char* str1, char* str2) {
	int len1 = strlen(str1);
	if (len1!=strlen(str2)) return 0;
	return  (strncmp(str1,str2,len1)==0);
}
static void concat(char* str1, char* str2, int maxlen) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	if (len1+len2+1>maxlen) puts("concat overstepped its bounds"); // TODO: lock this down
	strncpy(str1+len1, str2, len2);
	str1[len1+len2] = 0;
}
static char* copy_string(char* str) {
	int len = strlen(str);
	char* copy = malloc(sizeof(char)*(len+1));
	strcpy(copy, str);
	copy[len] = '\0';
	return copy;
} // NOTE: caller must free() result!

///////////////////////////////////////

static int hide(char* name) {
	if (name[0]=='.') return 1;
	
	// TODO: these could be removed if we dropped support for MainUI
	
	// if (match_suffix(".hide", name)) return 1;
	// if (match_suffix(".zip", name)) return 1;
	if (match_suffix("_cache.db", name)) return 1;

	// if (exact_match("cachefile", name)) return 1;
	// if (exact_match("update.log", name)) return 1;
	// if (exact_match("Apps", name)) return 1;
	// if (exact_match("Imgs", name)) return 1;
	if (match_prefix("COPYING", name)) return 1;
	if (exact_match("license", name)) return 1;
	if (exact_match("LICENSE", name)) return 1;

	return 0;
}

///////////////////////////////////////

typedef struct Array {
	int count;
	int capacity;
	void** items;
} Array;
static Array* Array_new(void) {
	Array* self = malloc(sizeof(Array));
	self->count = 0;
	self->capacity = 8;
	self->items = malloc(sizeof(void*) * self->capacity);
	return self;
}
static void Array_push(Array* self, void* item) {
	if (self->count>=self->capacity) {
		self->capacity *= 2;
		self->items = realloc(self->items, sizeof(void*) * self->capacity);
	}
	self->items[self->count++] = item;
}
static void Array_unshift(Array* self, void* item) {
	if (self->count==0) return Array_push(self, item);
	Array_push(self, NULL); // ensures we have enough capacity
	for (int i=self->count-2; i>=0; i--) {
		self->items[i+1] = self->items[i];
	}
	self->items[0] = item;
}
static void* Array_pop(Array* self) {
	if (self->count==0) return NULL;
	return self->items[--self->count];
} // NOTE: caller must free result (when appropriate)!
static void Array_free(Array* self) {
	free(self->items); // NOTE: caller is responsible for freeing individual items first!
	free(self);
}
static void Array_reverse(Array* self) {
	int end = self->count-1;
	int mid = self->count/2;
	for (int i=0; i<mid; i++) {
		void* item = self->items[i];
		self->items[i] = self->items[end-i];
		self->items[end-i] = item;
	}
}

///////////////////////////////////////

enum EntryType {
	kEntryDir,
	kEntryPak,
	kEntryRom,
};
typedef struct Entry {
	char* path;
	char* name;
	int type;
} Entry;

static char* raw_name(char* path) {
	char* tmp;
	char name[128];
	tmp = strrchr(path, '/')+1;
	strcpy(name, tmp); // filename

	tmp = strrchr(name, '.');
	if (tmp!=NULL) tmp[0] = '\0'; // remove extension
	
	// remove trailing parens (round and square)
	char safe[128];
	strcpy(safe,name);
	while ((tmp=strrchr(name, '('))!=NULL || (tmp=strrchr(name, '['))!=NULL) {
		tmp[0] = '\0';
		tmp = name;
	}
	if (name[0]=='\0') strcpy(name,safe);
	
	// else concat(name, "/", 128);
	return copy_string(name);
} // NOTE: caller must free() result!

static Entry* Entry_new(char* path, int type) {
	Entry* self = malloc(sizeof(Entry));
	self->type = type;
	self->name = raw_name(path);
	self->path = copy_string(path);
	return self;
}
static void Entry_free(Entry* self) {
	free(self->path);
	free(self->name);
	free(self);
}
static int EntryArray_indexOf(Array* self, char* path) {
	for (int i=0; i<self->count; i++) {
		Entry* entry = self->items[i];
		if (exact_match(entry->path, path)) return i;
	}
	return -1;
}
static int EntryArray_sortEntry(const void* a, const void* b) {
	Entry* item1 = *(Entry**)a;
	Entry* item2 = *(Entry**)b;
	return strcasecmp(item1->name, item2->name);
}
static void EntryArray_sort(Array* self) {
	qsort(self->items, self->count, sizeof(void*), EntryArray_sortEntry);
}

static void EntryArray_free(Array* self) {
	for (int i=0; i<self->count; i++) {
		Entry_free(self->items[i]);
	}
	Array_free(self);
}

///////////////////////////////////////

static int exists(char* path) {
	return access(path, F_OK)==0;
}

static void StringArray_free(Array* self) {
	for (int i=0; i<self->count; i++) {
		free(self->items[i]);
	}
	Array_free(self);
}
static int StringArray_indexOf(Array* self, char* str) {
	for (int i=0; i<self->count; i++) {
		if (exact_match(self->items[i], str)) return i;
	}
	return -1;
}

#define kMaxRecents 15
Array* recents;
static int hasRecents(void) {
	int has = 0;
	
	FILE* file = fopen(kRootDir "/recent.txt", "r"); // newest at top
	if (file) {
		char line[256];
		while (fgets(line,256,file)!=NULL) {
			int len = strlen(line);
			if (len>0 && line[len-1]=='\n') line[len-1] = 0; // trim newline
			if (strlen(line)==0) continue; // skip empty lines
			if (exists(line)) {
				has = 1;
				if (recents->count<kMaxRecents) {
					Array_push(recents, copy_string(line));
				}
			}
		}
		fclose(file);
	}
	return has;
}
static int hasPaks(char* path) {
	int has = 0;

	DIR *dh = opendir(path);
	if (dh!=NULL) {
		struct dirent *dp;
		while((dp = readdir(dh)) != NULL) {
			int is_dir = dp->d_type==DT_DIR;
			if (is_dir && match_suffix(".pak", dp->d_name)) {
				char pak[256];
				pak[0] = '\0';
				concat(pak, path, 256);
				concat(pak, "/", 256);
				concat(pak, dp->d_name, 256);
				concat(pak, "/launch.sh", 256);
				if (exists(pak)) {
					has = 1;
					break;
				}
			}
		}
		closedir(dh);
	}
	return has;
}
static int hasRoms(char* path) {
	int has = 0;
	
	// makes sure we have an emu pak
	char emu[256];
	strcpy(emu, path);
	char* emus = kRootDir "/Emus/";
	strncpy(emu, emus, strlen(emus));
	concat(emu, ".pak/launch.sh", 256);
	if (!exists(emu)) return has;
	
	// now look for at least one rom
	DIR *dh = opendir(path);
	if (dh!=NULL) {
		struct dirent *dp;
		while((dp = readdir(dh)) != NULL) {
			if (hide(dp->d_name)) continue;
			if (dp->d_type==DT_DIR) continue;
			has = 1;
			break;
		}
		closedir(dh);
	}
	return has;
}

static Array* getRecents(void) {
	Array* entries = Array_new();
	for (int i=0; i<recents->count; i++) {
		char* path = recents->items[i];
		int type = match_suffix(".pak", path) ? kEntryPak : kEntryRom;
		Array_push(entries, Entry_new(path, type));
	}
	return entries;
}
static Array* getEntries(char* path) {
	Array* entries = Array_new();
	DIR *dh = opendir(path);
	if (dh!=NULL) {
		struct dirent *dp;
		char full_path[256];
		full_path[0] = '\0';
		concat(full_path, path, 256);
		concat(full_path, "/", 256);
		char* tmp = full_path + strlen(full_path);
		while((dp = readdir(dh)) != NULL) {
			if (hide(dp->d_name)) continue;
			strcpy(tmp, dp->d_name);
			tmp[strlen(dp->d_name)] = '\0';
			int is_dir = dp->d_type==DT_DIR;
			int type;
			if (is_dir) {
				if (match_suffix(".pak", dp->d_name)) {
					type = kEntryPak;
				}
				else {
					type = kEntryDir;
				}
			}
			else {
				type = kEntryRom;
			}
			Array_push(entries, Entry_new(full_path, type));
		}
		closedir(dh);
	}
	EntryArray_sort(entries);
	return entries;
}
static Array* getRoot(void) {
	Array* entries = Array_new();

	int has_recents = hasRecents();
	int has_games = hasPaks(kRootDir "/Games");
	int has_tools = hasPaks(kRootDir "/Tools");
	
	if (has_recents) Array_push(entries, Entry_new(kRecentlyPlayedDir, kEntryDir));
	
	char* path = kRootDir "/Roms";
	DIR *dh = opendir(path);
	if (dh!=NULL) {
		struct dirent *dp;
		char full_path[256];
		full_path[0] = '\0';
		concat(full_path, path, 256);
		concat(full_path, "/", 256);
		char* tmp = full_path + strlen(full_path);
		Array* emus = Array_new();
		while((dp = readdir(dh)) != NULL) {
			if (hide(dp->d_name)) continue;
			strcpy(tmp, dp->d_name);
			tmp[strlen(dp->d_name)] = '\0';
			
			if (hasRoms(full_path)) Array_push(emus, Entry_new(full_path, kEntryDir));
		}
		EntryArray_sort(emus);
		for (int i=0; i<emus->count; i++) {
			Array_push(entries, emus->items[i]);
		}
		Array_free(emus); // just free the array part, root now owns emus entries
		closedir(dh);
	}
	
	if (has_games) Array_push(entries, Entry_new(kRootDir "/Games", kEntryDir));
	if (has_tools) Array_push(entries, Entry_new(kRootDir "/Tools", kEntryDir));
	
	return entries;
}

static void saveRecents(void) {
	FILE* file = fopen(kRootDir "/recent.txt", "w");
	if (file) {
		for (int i=0; i<recents->count; i++) {
			fputs(recents->items[i], file);
			putc('\n', file);
		}
		fclose(file);
	}
}
static void addRecent(char* path) {
	int id = StringArray_indexOf(recents, path);
	if (id==-1) { // add
		while (recents->count>=kMaxRecents) {
			free(Array_pop(recents));
		}
		Array_unshift(recents, copy_string(path));
	}
	else if (id>0) { // bump to top
		for (int i=id; i>0; i--) {
			void* tmp = recents->items[i-1];
			recents->items[i-1] = recents->items[i];
			recents->items[i] = tmp;
		}
	}
	saveRecents();
}

///////////////////////////////////////

// TODO: each Directory needs to maintain its own selected index
typedef struct Directory {
	char* path;
	Array* entries;
	// rendering
	int selected;
	int start;
	int end;
} Directory;
static Directory* Directory_new(char* path, int selected) {
	Directory* self = malloc(sizeof(Directory));
	self->path = copy_string(path);
	if (exact_match(path, kRootDir)) {
		self->entries = getRoot();
	}
	else if (exact_match(path, kRecentlyPlayedDir)) {
		self->entries = getRecents();
	}
	else {
		self->entries = getEntries(path);
	}
	self->selected = selected;
	return self;
}
static void Directory_free(Directory* self) {
	free(self->path);
	EntryArray_free(self->entries);
	free(self);
}
static void DirectoryArray_pop(Array* self) {
	Directory_free(Array_pop(self));
}
static void DirectoryArray_free(Array* self) {
	for (int i=0; i<self->count; i++) {
		Directory_free(self->items[i]);
	}
	Array_free(self);
}

///////////////////////////////////////

typedef struct ButtonState {
	int justPressed;
	int justRepeated;
	int isPressed;
	int justReleased;
} ButtonState;
enum {
	TRIMUI_UP 		= SDLK_UP,
	TRIMUI_DOWN 	= SDLK_DOWN,
	TRIMUI_LEFT 	= SDLK_LEFT,
	TRIMUI_RIGHT 	= SDLK_RIGHT,
	TRIMUI_A 		= SDLK_SPACE,
	TRIMUI_B 		= SDLK_LCTRL,
	TRIMUI_X 		= SDLK_LSHIFT,
	TRIMUI_Y 		= SDLK_LALT,
	TRIMUI_START 	= SDLK_RETURN,
	TRIMUI_SELECT 	= SDLK_RCTRL,
	TRIMUI_L 		= SDLK_TAB,
	TRIMUI_R 		= SDLK_BACKSPACE,
	TRIMUI_MENU 	= SDLK_ESCAPE,
};
enum {
	kButtonNull = -1,
	kButtonUp,
	kButtonDown,
	kButtonLeft,
	kButtonRight,
	kButtonA,
	kButtonB,
	kButtonX,
	kButtonY,
	kButtonStart,
	kButtonSelect,
	kButtonL,
	kButtonR,
	kButtonMenu,
	kButtonCount,
};

// TODO: this should be 3 bitmasks, not 13 structs :shrug:
ButtonState buttons[kButtonCount];

static void Input_beforePoll(void) {
	for (int i=0; i<kButtonCount; i++) {
		buttons[i].justPressed = 0;
		buttons[i].justRepeated = 0;
	}
}
#define Input_justPressed(btn) buttons[(btn)].justPressed
#define Input_justRepeated(btn) buttons[(btn)].justRepeated
#define Input_isPressed(btn) buttons[(btn)].isPressed
#define Input_justReleased(btn) buttons[(btn)].justReleased
static int Input_getButton(SDL_Event *event) {
	switch(event->key.keysym.sym) {
		case TRIMUI_A: 		return kButtonA; break;
		case TRIMUI_B: 		return kButtonB; break;
		case TRIMUI_X: 		return kButtonX; break;
		case TRIMUI_Y: 		return kButtonY; break;
		case TRIMUI_START: 	return kButtonStart; break;
		case TRIMUI_SELECT:	return kButtonSelect; break;
		case TRIMUI_UP:		return kButtonUp; break;
		case TRIMUI_DOWN:	return kButtonDown; break;
		case TRIMUI_LEFT:	return kButtonLeft; break;
		case TRIMUI_RIGHT:	return kButtonRight; break;
		case TRIMUI_L:		return kButtonL; break;
		case TRIMUI_R:		return kButtonR; break;
		case TRIMUI_MENU:	return kButtonMenu; break;
	}
	return kButtonNull;
}

///////////////////////////////////////

static void restoreSettings(void) {
	// NOTE: this must be called AFTER we get what we need from shared memory via GetKeyShm()
	// TODO: maybe not true anymore?
	int		address = 0x01c20890;
	int		pagesize = sysconf(_SC_PAGESIZE);
	int		addrmask1 = address & (0-pagesize);
	int		addrmask2 = address & (pagesize-1);
	int		memhandle = open("/dev/mem",O_RDWR|O_SYNC);	//0x00101002 = O_RDWR + something unnecessary
	unsigned char	*memaddress = mmap(NULL,pagesize,PROT_READ|PROT_WRITE,MAP_SHARED,memhandle,addrmask1);
	unsigned char	*modaddress = (memaddress + addrmask2);
	int		moddata = *(unsigned int*)modaddress;
	if ((moddata & 1) != 0) { *(unsigned int*)modaddress = moddata & 0xF0FFFFFF | 0x03000000 ; }
	munmap(memaddress,pagesize);
	close(memhandle);
	
	void* libtinyalsa = dlopen("/usr/lib/libtinyalsa.so", RTLD_NOW|RTLD_GLOBAL);
	void* libshmvar = dlopen("/usr/trimui/lib/libshmvar.so", RTLD_NOW|RTLD_GLOBAL);
	void* libtmenu = dlopen("/usr/trimui/lib/libtmenu.so", RTLD_LAZY);

	void (*setLCDBrightness)(int) = dlsym(libtmenu, "setLCDBrightness");
	void* (*setVolume)(int) = dlsym(libtmenu, "sunxi_set_volume");

	void (*InitKeyShm)(int* [10]) = dlsym(libshmvar, "InitKeyShm");
	int (*GetKeyShm)(void*,int) = dlsym(libshmvar, "GetKeyShm");

	int volume = 10;
	int brightness = 5;

	int* key[10];
	InitKeyShm(key);
	volume = GetKeyShm(key, 0);
	brightness = GetKeyShm(key, 1);

	setVolume(volume);
	setLCDBrightness(brightness);

	dlclose(libshmvar);
	dlclose(libtmenu);
	dlclose(libtinyalsa);
}
static int getBatteryLevel(void) {
	char value[16];
	memset(value, '\0', 16);
	FILE* file = fopen("/sys/devices/soc/1c23400.battery/adc", "r");
	if (file!=NULL) {
		fgets(value, 16, file);
		fclose(file);
	}
	return atol(value);
}

///////////////////////////////////////

SDL_Surface* screen;
SDL_Surface* buffer;
int quit = 0;

Array* stack;
Directory* top;
#define kMaxRows 5

static void saveLast(char* path) {
	// special case for recently played
	if (exact_match(top->path, kRecentlyPlayedDir)) {
		// NOTE: that we don't have to save the file because
		// your most recently played game will always be at
		// the top which is also the default selection
		path = kRecentlyPlayedDir;
	}
	
	FILE* file = fopen(kLastPath, "w");
	if (file) {
		fputs(path, file);
		fclose(file);
	}
}

static void queue_next(char* cmd) {
	// TODO: add to recents
	// queue up next command
	FILE* file = fopen(kRootDir "/System.pak/next.sh", "w");
	if (file) {
		int len = strlen(cmd);
		fwrite(cmd,1,strlen(cmd),file);
		fclose(file);
		quit = 1;
	}
}
static void open_directory(char* path) {
	top = Directory_new(path, 0);
	// TODO: move start and end to Directory?
	top->start = 0;
	top->end = (top->entries->count<kMaxRows) ? top->entries->count : kMaxRows;
	Array_push(stack, top);
}
static void close_directory(void) {
	DirectoryArray_pop(stack);
	top = stack->items[stack->count-1];
}
	
static void Entry_open(Entry* self) {
	char launch[256];
	launch[0] = '"';
	if (self->type==kEntryRom) {
		strcpy(launch+1, self->path);
		char* slash = strrchr(launch, '/');
		launch[(slash-launch)] = '\0';
		char* emus = kRootDir "/Emus/";
		strncpy(launch+1, emus, strlen(emus));
		concat(launch, ".pak/launch.sh\" \"", 256);
		concat(launch, self->path, 256);
		concat(launch, "\"", 256);
		addRecent(self->path);
		saveLast(self->path);
		queue_next(launch);
	}
	else if (self->type==kEntryPak) {
		strcpy(launch+1, self->path);
		concat(launch, "/launch.sh\"", 256);
		if (match_prefix(kRootDir "/Games", self->path)) {
			addRecent(self->path);
		}
		saveLast(self->path);
		queue_next(launch);
	}
	else if (self->type==kEntryDir) {
		open_directory(self->path);
	}
}

static void loadLast(void) { // call after loading root directory
	FILE* file = fopen(kLastPath, "r");
	if (file) {
		char line[256];
		line[0] = 0;
		while (fgets(line,256,file)!=NULL) {
			int len = strlen(line);
			if (len>0 && line[len-1]=='\n') line[len-1] = 0; // trim newline
			if (strlen(line)==0) continue; // skip empty lines
			if (exists(line)) break; // line is now our last opened thing
		}
		fclose(file);
		if (strlen(line)>0) {
			Array* last = Array_new();
			while (!exact_match(line, kRootDir)) {
				Array_push(last, copy_string(line));
				
				char* slash = strrchr(line, '/');
				line[(slash-line)] = '\0';
			}
			
			while (last->count>0) {
				char* path = Array_pop(last);
				for (int i=0; i<top->entries->count; i++) {
					Entry* entry = top->entries->items[i];
					if (exact_match(entry->path, path)) {
						top->selected = i;
						if (i>=top->end) {
							top->start = i;
							top->end = top->start + kMaxRows;
							if (top->end>top->entries->count) {
								top->end = top->entries->count;
								top->start = top->end - kMaxRows;
							}
						}
						if (entry->type==kEntryDir) {
							open_directory(entry->path);
						}
					}
				}
				free(path); // we took ownership when we popped it
			}
			
			StringArray_free(last);
		}
	}
}

static void Menu_init(void) {
	stack = Array_new(); // array of open Directories
	recents = Array_new();
	
	open_directory(kRootDir);
	loadLast(); // restore state when available
}
static void Menu_quit(void) {
	StringArray_free(recents);
	DirectoryArray_free(stack);
}

void render(void) {
	
}

int main(void) {
	// freopen(kRootDir "/stderr.txt", "w", stderr);
	// freopen(kRootDir "/stdout.txt", "w", stdout);
	signal(SIGSEGV, error_handler); // runtime error reporting
	
	if (access("/dev/dsp1", 4)==0) putenv("AUDIODEV=/dev/dsp1"); // assuming headphones
	else putenv("AUDIODEV=/dev/dsp"); // speaker
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)==-1) {
		puts("could not init SDL");
		puts(SDL_GetError());
		// fflush(stdout);
	}
	
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)==-1) {
	    printf( "Mix_OpenAudio() failed! SDL_mixer Error: %s\n", Mix_GetError() );
		// fflush(stdout);
		
		puts("\tquitting prematurely");
		// fflush(stdout);
		
		Mix_CloseAudio();
		Mix_Quit();
		SDL_Quit();
		fclose(stdout);
		SDL_Quit();
		return 0;
	}
	
	screen = SDL_SetVideoMode(320, 240, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
	buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0, 0, 0, 0);
	
	SDL_ShowCursor(0);
	SDL_EnableKeyRepeat(500,150);
	
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("/usr/res/BPreplayBold.otf", 16);
	TTF_Font* tiny = TTF_OpenFont("/usr/res/BPreplayBold.otf", 14);
	SDL_Color color = {0xff,0xff,0xff};
	
	restoreSettings();

	// TODO: free all these!
	SDL_Surface* ui_logo = IMG_Load("res/logo.png");
	SDL_Surface* ui_highlight_bar = IMG_Load("/usr/trimui/res/skin/list-selected-bg.png");
	SDL_Surface* ui_top_bar = IMG_Load("/usr/trimui/res/skin/title-bg.png");
	SDL_Surface* ui_bottom_bar = IMG_Load("/usr/trimui/res/skin/tips-bar-bg.png");
	SDL_Surface* ui_browse_icon = IMG_Load("/usr/trimui/res/skin/stat-nav-icon.png");
	SDL_Surface* ui_round_button = IMG_Load("res/nav-bar-item-bg.png");
	
	SDL_Surface* ui_power_0_icon   = IMG_Load("/usr/trimui/res/skin/power-0%-icon.png");
	SDL_Surface* ui_power_20_icon  = IMG_Load("/usr/trimui/res/skin/power-20%-icon.png");
	SDL_Surface* ui_power_50_icon  = IMG_Load("/usr/trimui/res/skin/power-50%-icon.png");
	SDL_Surface* ui_power_80_icon  = IMG_Load("/usr/trimui/res/skin/power-80%-icon.png");
	SDL_Surface* ui_power_100_icon = IMG_Load("/usr/trimui/res/skin/power-full-icon.png");

	// Mix_Chunk *click = Mix_LoadWAV("/usr/trimui/res/sound/click.wav");

	Menu_init();
	
	SDL_FillRect(buffer, &buffer->clip_rect, SDL_MapRGB(buffer->format, 0, 0, 0));
	
	SDL_Event event;
	int is_dirty = 1;
	while (!quit) {
		Input_beforePoll();
		while (SDL_PollEvent(&event)) {
			int btn;
			switch( event.type ){
				case SDL_KEYDOWN:
					btn = Input_getButton(&event);
					if (btn==kButtonNull) continue;

					buttons[btn].justRepeated = 1;
					if (!buttons[btn].isPressed) {
						buttons[btn].justPressed = 1;
						buttons[btn].isPressed = 1;
					}
					// Mix_PlayChannel(-1, click, 0);
				break;
				
				case SDL_KEYUP:
					btn = Input_getButton(&event);
					if (btn==kButtonNull) continue;
					
					buttons[btn].isPressed = 0;
					buttons[btn].justReleased = 1;
				break;
			}
		}
		
		int selected = top->selected;
		int total = top->entries->count;
		int dir = 0;
		if (Input_justRepeated(kButtonUp)) {
			dir = -1;
			selected -= 1;
			if (selected<0) {
				selected = total-1;
				int start = total - kMaxRows;
				top->start = (start<0) ? 0 : start;
				top->end = total;
			}
			else if (selected<top->start) {
				top->start -= 1;
				top->end -= 1;
			}
		}
		else if (Input_justRepeated(kButtonDown)) {
			dir = 1;
			selected += 1;
			if (selected>=total) {
				selected = 0;
				top->start = 0;
				top->end = (total<kMaxRows) ? total : kMaxRows;
			}
			else if (selected>=top->end) {
				top->start += 1;
				top->end += 1;
			}
		}
		if (Input_justPressed(kButtonLeft)) {
			selected -= kMaxRows;
			if (selected<0) {
				selected = 0;
				top->start = 0;
				top->end = (total<kMaxRows) ? total : kMaxRows;
			}
			else if (selected<top->start) {
				top->start -= kMaxRows;
				if (top->start<0) top->start = 0;
				top->end = top->start + kMaxRows;
			}
		}
		else if (Input_justPressed(kButtonRight)) {
			selected += kMaxRows;
			if (selected>=total) {
				selected = total-1;
				int start = total - kMaxRows;
				top->start = (start<0) ? 0 : start;
				top->end = total;
			}
			else if (selected>=top->end) {
				top->end += kMaxRows;
				if (top->end>total) top->end = total;
				top->start = top->end - kMaxRows;
			}
		}

		if (selected!=top->selected) {
			top->selected = selected;
			is_dirty = 1;
		}
		
		if (Input_justPressed(kButtonA)) {
			Entry_open(top->entries->items[top->selected]);
			is_dirty = 1;
		}
		if (Input_justPressed(kButtonB) && stack->count>1) {
			close_directory();
			is_dirty = 1;
		}
		
		// if (Input_justPressed(kButtonMenu)) {
		// 	queue_next("/usr/trimui/bin/StockUI");
		// 	break;
		// }
		
		if (is_dirty) {
			
			// clear
			SDL_FillRect(buffer, &buffer->clip_rect, SDL_MapRGB(buffer->format, 0,0,0));
			
			// chrome
			SDL_BlitSurface(ui_top_bar, NULL, buffer, NULL);
			SDL_BlitSurface(ui_bottom_bar, NULL, buffer, &(SDL_Rect){0,202,0,0});
			
			SDL_Surface* text;
			// x/y text
			char mini[8];
			sprintf(mini, "/%d", top->entries->count);
			text = TTF_RenderUTF8_Blended(tiny, mini, (SDL_Color){0xd2,0xb4,0x6c});
			SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){184,9,0,0});
			SDL_FreeSurface(text);
			
			sprintf(mini, "%d", top->selected+1);
			text = TTF_RenderUTF8_Blended(tiny, mini, (SDL_Color){0xd2,0xb4,0x6c});
			SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){184-text->w,9,0,0});
			SDL_FreeSurface(text);

			// logo
			SDL_BlitSurface(ui_logo, NULL, buffer, &(SDL_Rect){10,10,0,0});
			
			// battery
			int charge = getBatteryLevel();
			SDL_Surface* ui_power_icon;
			if (charge<41)		ui_power_icon = ui_power_0_icon;
			else if (charge<43) ui_power_icon = ui_power_20_icon;
			else if (charge<44) ui_power_icon = ui_power_50_icon;
			else if (charge<46) ui_power_icon = ui_power_80_icon;
			else				ui_power_icon = ui_power_100_icon;
			SDL_BlitSurface(ui_power_icon, NULL, buffer, &(SDL_Rect){294,6,0,0});
			
			// browse
			SDL_BlitSurface(ui_browse_icon, NULL, buffer, &(SDL_Rect){10,210,0,0});
			text = TTF_RenderUTF8_Blended(tiny, "BROWSE", (SDL_Color){0xff,0xff,0xff});
			SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){36,212,0,0});
			SDL_FreeSurface(text);
			
			// A Open
			SDL_BlitSurface(ui_round_button, NULL, buffer, &(SDL_Rect){251,210,0,0});
			text = TTF_RenderUTF8_Blended(tiny, "OPEN", (SDL_Color){0xff,0xff,0xff});
			SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){276,212,0,0});
			SDL_FreeSurface(text);
			
			text = TTF_RenderUTF8_Blended(font, "A", (SDL_Color){0x9f,0x89,0x52});
			SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){251+6,210+1,0,0});
			SDL_FreeSurface(text);
			
			// B Back
			if (stack->count>1) {
				SDL_BlitSurface(ui_round_button, NULL, buffer, &(SDL_Rect){251-68,210,0,0});
				text = TTF_RenderUTF8_Blended(tiny, "BACK", (SDL_Color){0xff,0xff,0xff});
				SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){276-68,212,0,0});
				SDL_FreeSurface(text);
			
				text = TTF_RenderUTF8_Blended(font, "B", (SDL_Color){0x9f,0x89,0x52});
				SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){251+6-68+1,210+1,0,0});
				SDL_FreeSurface(text);
			}
			
			int y = 0;
			for (int i=top->start; i<top->end; i++) {
				Entry* entry = top->entries->items[i];

				if (top->selected==i) {
					// bar
					SDL_BlitSurface(ui_highlight_bar, NULL, buffer, &(SDL_Rect){0,38+y,0,0});
					// shadow
					text = TTF_RenderUTF8_Blended(font, entry->name, (SDL_Color){0x68,0x5a,0x35});
					SDL_BlitSurface(text, &(SDL_Rect){0,0,320-32,text->h}, buffer, &(SDL_Rect){16+1,38+y+6+2,0,0});
					SDL_FreeSurface(text);
				}
				
				text = TTF_RenderUTF8_Blended(font, entry->name, color);
				SDL_BlitSurface(text, &(SDL_Rect){0,0,320-32,text->h}, buffer, &(SDL_Rect){16,38+y+6,0,0});
				SDL_FreeSurface(text);
				y += 32;
			}
			
			SDL_BlitSurface(buffer, NULL, screen, NULL);
			SDL_Flip(screen);
			is_dirty = 0;
		}
	}
	
	// one last wipe
	SDL_FillRect(buffer, &buffer->clip_rect, SDL_MapRGB(buffer->format, 0, 0, 0));
	SDL_BlitSurface(buffer, NULL, screen, NULL);
	SDL_Flip(screen);
	
	Menu_quit();
	
	// Mix_FreeChunk(click);
	Mix_CloseAudio();
	Mix_Quit();
	
	SDL_FreeSurface(ui_logo);
	SDL_FreeSurface(ui_highlight_bar);
	SDL_FreeSurface(ui_top_bar);
	SDL_FreeSurface(ui_bottom_bar);
	SDL_FreeSurface(ui_browse_icon);
	SDL_FreeSurface(ui_round_button);
	SDL_FreeSurface(ui_power_0_icon);
	SDL_FreeSurface(ui_power_20_icon);
	SDL_FreeSurface(ui_power_50_icon);
	SDL_FreeSurface(ui_power_80_icon);
	SDL_FreeSurface(ui_power_100_icon);
	
	TTF_CloseFont(font);
	TTF_CloseFont(tiny);
	
	TTF_Quit();
	SDL_Quit();
	
	// fflush(stdout);
	// fclose(stdout);
	return 0;
}