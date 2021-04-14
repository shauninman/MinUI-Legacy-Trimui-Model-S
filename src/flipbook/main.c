#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

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

static void StringArray_free(Array* self) {
	for (int i=0; i<self->count; i++) {
		free(self->items[i]);
	}
	Array_free(self);
}
static int StringArray_sortString(const void* a, const void* b) {
	char* item1 = *(char**)a;
	char* item2 = *(char**)b;
	return strcasecmp(item1, item2);
}
static void StringArray_sort(Array* self) {
	qsort(self->items, self->count, sizeof(void*), StringArray_sortString);
}

///////////////////////////////////////

// NOTE: these are now case-insensitive!
static int match_prefix(char* pre, char* str) {
	return (strncasecmp(pre,str,strlen(pre))==0);
}
static int match_suffix(char* suf, char* str) {
	int len = strlen(suf);
	return (strncasecmp(suf, str+strlen(str)-len, len)==0);
}
// NOTE: this is still case-sensitive
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
	if (match_suffix("launch.sh", name)) return 1;
	return 0;
}

///////////////////////////////////////

int main(int argc , char* argv[]) {
	if (argc>2) {
		puts("Usage: flipbook [dirpath]");
		return 0;
	}
	
	if (SDL_Init(SDL_INIT_VIDEO)==-1) {
		puts("could not init SDL");
		puts(SDL_GetError());
		
	}
	
	SDL_Surface* screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
	SDL_ShowCursor(0);

	char path[256];
	strncpy(path, argc>1?argv[1]:".", 256);
	puts("flipbook");
	puts(path);
	
	Array* images = Array_new();
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
			puts(full_path);
			Array_push(images, copy_string(full_path));
		}
		StringArray_sort(images);
		closedir(dh);
	}
	
	int quit = 0;
	int is_dirty = 1;
	int idx = images->count; // will wrap to 0 on first draw
	SDL_Surface *image;
	SDL_Event event;
	while (!quit) {
		int last_idx = idx;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN: {
					SDLKey btn = event.key.keysym.sym;
					switch (event.key.keysym.sym) {
						case TRIMUI_A:
						case TRIMUI_RIGHT:
							idx += 1;
						break;
						
						case TRIMUI_B:
						case TRIMUI_LEFT:
							idx -= 1;
						break;
						
						case TRIMUI_UP:
							idx = 0;
						break;
						case TRIMUI_DOWN:
							idx = images->count-1;
						break;
						
						case TRIMUI_MENU:
							quit = 1;
						break;
						
					}
				} break;
			}
		}
		
		if (is_dirty || idx!=last_idx) {
			if (idx<0) idx += images->count;
			if (idx>=images->count) idx -= images->count;
			is_dirty = 1;
		}
		
		if (is_dirty) {
			image = IMG_Load(images->items[idx]);
			if (image==NULL) puts(IMG_GetError());
			SDL_BlitSurface(image, NULL, screen, NULL);
			SDL_Flip(screen);
			SDL_FreeSurface(image);
			is_dirty = 0;
		}
	}
	SDL_FillRect(screen, NULL, 0);
	SDL_Flip(screen);
	
	StringArray_free(images);
	
	IMG_Quit();
	SDL_Quit();
	return 0;
}