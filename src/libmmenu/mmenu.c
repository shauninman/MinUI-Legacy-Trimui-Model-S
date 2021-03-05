#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "mmenu.h"

#define TRIMUI_UP 		SDLK_UP
#define TRIMUI_DOWN 	SDLK_DOWN
#define TRIMUI_LEFT 	SDLK_LEFT
#define TRIMUI_RIGHT 	SDLK_RIGHT
#define TRIMUI_A 		SDLK_SPACE
#define TRIMUI_B 		SDLK_LCTRL
#define TRIMUI_X 		SDLK_LSHIFT
#define TRIMUI_Y 		SDLK_LALT
#define TRIMUI_START 	SDLK_RETURN
#define TRIMUI_SELECT 	SDLK_RCTRL
#define TRIMUI_L 		SDLK_TAB
#define TRIMUI_R 		SDLK_BACKSPACE
#define TRIMUI_MENU 	SDLK_ESCAPE

static SDL_Surface* screen;
static SDL_Surface* buffer;
static SDL_Surface* overlay;
static SDL_Surface* ui_top_bar;
static SDL_Surface* ui_bottom_bar;
static SDL_Surface* ui_menu_bg;
static SDL_Surface* ui_menu_bar;
static SDL_Surface* ui_slot_bg;
static SDL_Surface* ui_slot_overlay;
static SDL_Surface* ui_browse_icon;
static SDL_Surface* ui_round_button;
static SDL_Surface* ui_arrow_right;
static SDL_Surface* ui_arrow_right_w;
static SDL_Surface* ui_selected_dot;
static SDL_Surface* ui_empty_slot;
static SDL_Surface* ui_no_preview;

static TTF_Font* font;
static TTF_Font* tiny;

static SDL_Color gold = {0xd2,0xb4,0x6c};
static SDL_Color bronze = {0x9f,0x89,0x52};
static SDL_Color white = {0xff,0xff,0xff};

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
static void error_handler(int sig) {
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "mmenu Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}

__attribute__((constructor)) static void init(void) {
	signal(SIGSEGV, error_handler); // runtime error reporting

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	
	screen = SDL_SetVideoMode(320, 240, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
	buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0, 0, 0, 0);
	
	font = TTF_OpenFont("/usr/res/BPreplayBold.otf", 16);
	tiny = TTF_OpenFont("/usr/res/BPreplayBold.otf", 14);
	
	overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0, 0, 0, 0);
	SDL_SetAlpha(overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(overlay, NULL, 0);
	
	ui_top_bar = IMG_Load("/usr/trimui/res/skin/navbg.png");
	ui_bottom_bar = IMG_Load("/usr/trimui/res/skin/statbg.png");
	ui_menu_bg = IMG_Load("/usr/trimui/res/skin/menu-5line-bg.png");
	ui_menu_bar = IMG_Load("/usr/trimui/res/skin/list-item-select-bg-short.png");
	ui_slot_bg = IMG_Load("/mnt/SDCARD/System.pak/res/save-slot-bg.png");
	ui_slot_overlay = IMG_Load("/mnt/SDCARD/System.pak/res/save-slot-overlay.png");
	
	ui_browse_icon = IMG_Load("/usr/trimui/res/skin/stat-nav-icon.png");
	ui_round_button = IMG_Load("/mnt/SDCARD/System.pak/res/nav-bar-item-bg.png");
	
	ui_arrow_right = IMG_Load("/usr/trimui/res/skin/right-arrow-icon-normal-small.png");
	ui_arrow_right_w = IMG_Load("/usr/trimui/res/skin/right-arrow-small.png");
	ui_selected_dot = IMG_Load("/mnt/SDCARD/System.pak/res/selected-slot-dot.png");
	
	ui_empty_slot = TTF_RenderUTF8_Blended(tiny, "Empty Slot", gold);
	ui_no_preview = TTF_RenderUTF8_Blended(tiny, "No Preview", gold);
}

__attribute__((destructor)) static void quit(void) {
	SDL_FreeSurface(ui_empty_slot);
	SDL_FreeSurface(ui_no_preview);
	SDL_FreeSurface(ui_selected_dot);
	SDL_FreeSurface(ui_arrow_right);
	SDL_FreeSurface(ui_arrow_right_w);
	SDL_FreeSurface(ui_browse_icon);
	SDL_FreeSurface(ui_round_button);
	SDL_FreeSurface(ui_slot_overlay);
	SDL_FreeSurface(ui_slot_bg);
	SDL_FreeSurface(ui_menu_bar);
	SDL_FreeSurface(ui_menu_bg);
	SDL_FreeSurface(ui_bottom_bar);
	SDL_FreeSurface(ui_top_bar);
	SDL_FreeSurface(overlay);
	SDL_FreeSurface(buffer);
	SDL_FreeSurface(screen);
	
	TTF_CloseFont(tiny);
	TTF_CloseFont(font);
	
	TTF_Quit();
	SDL_Quit();
}

#define kItemCount 5
char* items[kItemCount] = {
	"Continue",
	"Save",
	"Load",
	"Advanced",
	"Exit Game",
};
enum {
	kItemContinue,
	kItemSave,
	kItemLoad,
	kItemAdvanced,
	kItemExitGame,
};

typedef struct __attribute__((__packed__)) uint24_t {
	uint8_t a,b,c;
} uint24_t;
static SDL_Surface* thumbnail(SDL_Surface* src_img) {
	unsigned long then = SDL_GetTicks();
	SDL_Surface* dst_img = SDL_CreateRGBSurface(0,160,120,src_img->format->BitsPerPixel,src_img->format->Rmask,src_img->format->Gmask,src_img->format->Bmask,src_img->format->Amask);

	uint8_t* src_px = src_img->pixels;
	uint8_t* dst_px = dst_img->pixels;
	int step = dst_img->format->BytesPerPixel;
	int step2 = step * 2;
	int stride = src_img->pitch;
	// SDL_LockSurface(dst_img);
	for (int y=0; y<dst_img->h; y++) {
		for (int x=0; x<dst_img->w; x++) {
			switch(step) {
				case 1:
					*dst_px = *src_px;
					break;
				case 2:
					*(uint16_t*)dst_px = *(uint16_t*)src_px;
					break;
				case 3:
					*(uint24_t*)dst_px = *(uint24_t*)src_px;
					break;
				case 4:
					*(uint32_t*)dst_px = *(uint32_t*)src_px;
					break;
			}
			dst_px += step;
			src_px += step2;
		}
		src_px += stride;
	}
	// SDL_UnlockSurface(dst_img);
	printf("duration %lums\n", SDL_GetTicks() - then); // 3ms on device, 2ms with -O3/-Ofast

	return dst_img;
}

#define kSlotCount 8
static int slot = 0;

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* frame, MenuReturnEvent keyEvent) {	
	SDL_Surface* text;
	SDL_Surface* copy = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0, 0, 0, 0);	
	SDL_BlitSurface(frame, NULL, copy, NULL);
	
	char* tmp;
	char rom_file[128]; // with extension
	char rom_name[128]; // without
	
	tmp = strrchr(rom_path,'/');
	if (tmp==NULL) tmp = rom_path;
	else tmp += 1;

	strcpy(rom_name, tmp);
	strcpy(rom_file, tmp);
	tmp = strrchr(rom_name, '.');
	if (tmp!=NULL) tmp[0] = '\0';
	
	// remove trailing parens (round and square)
	char safe[128];
	strcpy(safe,rom_name);
	while ((tmp=strrchr(rom_name, '('))!=NULL || (tmp=strrchr(rom_name, '['))!=NULL) {
		tmp[0] = '\0';
		tmp = rom_name;
	}
	if (rom_name[0]=='\0') strcpy(rom_name,safe);
	
	char bmp_dir[128]; // /full/path/to/rom_dir/.mmenu
	strcpy(bmp_dir, rom_path);
	tmp = strrchr(bmp_dir,'/');
	if (tmp==NULL) tmp = bmp_dir;
	else tmp += 1;
	strcpy(tmp, ".mmenu");
	mkdir(bmp_dir, 0755);
	
	int status = kStatusContinue;
	int selected = 0; // resets every launch
	
	char save_path[256];
	char bmp_path[324];
	
	int quit = 0;
	int acted = 0;
	int is_dirty = 1;
	int save_exists = 0;
	int preview_exists = 0;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch( event.type ){
				case SDL_KEYUP: {
					if (acted && keyEvent==kMenuEventKeyUp) {
						SDLKey key = event.key.keysym.sym;
						if (key==TRIMUI_B || key==TRIMUI_A) {
							quit = 1;
						}
					}
				} break;
				case SDL_KEYDOWN: {
					if (acted) break;
					
					SDLKey key = event.key.keysym.sym;
					if (key==TRIMUI_UP) {
						selected -= 1;
						if (selected<0) selected += kItemCount;
						is_dirty = 1;
					}
					else if (key==TRIMUI_DOWN) {
						selected += 1;
						if (selected==kItemCount) selected -= kItemCount;
						is_dirty = 1;
					}
					else if (key==TRIMUI_LEFT) {
						slot -= 1;
						if (slot<0) slot += kSlotCount;
						is_dirty = 1;
					}
					else if (key==TRIMUI_RIGHT) {
						slot += 1;
						if (slot==kSlotCount) slot -= kSlotCount;
						is_dirty = 1;
					}
				
					if (is_dirty && (selected==kItemSave || selected==kItemLoad)) {
						sprintf(save_path, save_path_template, slot);
						sprintf(bmp_path, "%s/%s.%d.bmp", bmp_dir, rom_file, slot);
					
						save_exists = access(save_path, F_OK)==0;
						preview_exists = save_exists && access(bmp_path, F_OK)==0;
					}
				
					if (key==TRIMUI_MENU || key==TRIMUI_B) {
						if (keyEvent==kMenuEventKeyDown) quit = 1;
						else acted = 1;
						
						status = kStatusContinue;
						is_dirty = 1;
					}
					else if (key==TRIMUI_A) {
						if (selected==kItemLoad && !save_exists) break;
				
						if (keyEvent==kMenuEventKeyDown) quit = 1;
						else acted = 1;
						
						switch(selected) {
							case kItemContinue:
								status = kStatusContinue;
							break;
							case kItemSave:
								status = kStatusSaveSlot + slot;
								{
									SDL_Surface* preview = thumbnail(copy);
									SDL_RWops* out = SDL_RWFromFile(bmp_path, "wb");
									SDL_SaveBMP_RW(preview, out, 1);
									SDL_FreeSurface(preview);
								}
							break;
							case kItemLoad:
								status = kStatusLoadSlot + slot;
							break;
							case kItemAdvanced:
								status = kStatusOpenMenu;
							break;
							case kItemExitGame:
								status = kStatusExitGame;
							break;
						}
						
						is_dirty = 1;
					}
				} break;
			}
		}
		
		if (is_dirty) {
			if (acted) {
				// draw emu screen immediately so the wait for keyup feels like emu delay (because it is)
				SDL_BlitSurface(copy, NULL, buffer, NULL);
			}
			else {
				// ui
				SDL_BlitSurface(copy, NULL, buffer, NULL); // full screen image effectively clears buffer
				SDL_BlitSurface(overlay, NULL, buffer, NULL);
				SDL_BlitSurface(ui_top_bar, NULL, buffer, NULL);
				SDL_BlitSurface(ui_bottom_bar, NULL, buffer, &(SDL_Rect){0,210,0,0});
			
				// game name
				text = TTF_RenderUTF8_Blended(tiny, rom_name, gold);
				SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){(320-text->w)/2,6,0,0});
				SDL_FreeSurface(text);
			
				// menu
				{
					SDL_BlitSurface(ui_menu_bg, NULL, buffer, &(SDL_Rect){6,71,0,0});
	
					// menu
					int x = 14;
					int y = 75;
					for (int i=0; i<kItemCount; i++) {
						char* item = items[i];
						SDL_Color color = gold;
						if (i==selected) {
							SDL_BlitSurface(ui_menu_bar, NULL, buffer, &(SDL_Rect){6,y,0,0});
							color = white;
						}
		
						text = TTF_RenderUTF8_Blended(font, item, color);
						SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){x,y+4,0,0});
						SDL_FreeSurface(text);
				
						if (i==kItemSave || i==kItemLoad) {
							SDL_BlitSurface(i==selected?ui_arrow_right_w:ui_arrow_right, NULL, buffer, &(SDL_Rect){132,y+8,0,0});
						}
		
						y += 25;
					}
				}
			
				// slot preview
				if (selected==kItemSave || selected==kItemLoad) {
					SDL_BlitSurface(ui_slot_bg, NULL, buffer, &(SDL_Rect){148,71,0,0});
				
					if (preview_exists) { // has save, has preview
						SDL_Surface* preview = IMG_Load(bmp_path);
						SDL_BlitSurface(preview, NULL, buffer, &(SDL_Rect){151,74,0,0});
						SDL_FreeSurface(preview);
					}
					else if (save_exists) { // has save, no preview
						SDL_BlitSurface(ui_no_preview, NULL, buffer, &(SDL_Rect){151+(160-ui_no_preview->w)/2,126,0,0});
					}
					else {
						SDL_BlitSurface(ui_empty_slot, NULL, buffer, &(SDL_Rect){151+(160-ui_empty_slot->w)/2,126,0,0});
					}
					SDL_BlitSurface(ui_slot_overlay, NULL, buffer, &(SDL_Rect){151,74,0,0});
				
					SDL_BlitSurface(ui_selected_dot, NULL, buffer, &(SDL_Rect){200+(slot * 8),197,0,0});
				}
			
				// hints
				{
					// browse
					SDL_BlitSurface(ui_browse_icon, NULL, buffer, &(SDL_Rect){10,218-1,0,0});
					text = TTF_RenderUTF8_Blended(tiny, "BROWSE", white);
					SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){36,220-1,0,0});
					SDL_FreeSurface(text);
	
					// A (varies)
					SDL_BlitSurface(ui_round_button, NULL, buffer, &(SDL_Rect){10+251,218-1,0,0});
					text = TTF_RenderUTF8_Blended(font, "A", bronze);
					SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){10+251+6,218,0,0});
					SDL_FreeSurface(text);
	
					text = TTF_RenderUTF8_Blended(tiny, "ACT", white);
					SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){10+276,220-1,0,0});
					SDL_FreeSurface(text);
	
					// B Back
					SDL_BlitSurface(ui_round_button, NULL, buffer, &(SDL_Rect){10+251-68,218-1,0,0});
					text = TTF_RenderUTF8_Blended(font, "B", bronze);
					SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){10+251+6-68+1,218,0,0});
					SDL_FreeSurface(text);

					text = TTF_RenderUTF8_Blended(tiny, "BACK", white);
					SDL_BlitSurface(text, NULL, buffer, &(SDL_Rect){10+276-68,220-1,0,0});
					SDL_FreeSurface(text);
				}
			}
	
			SDL_BlitSurface(buffer, NULL, screen, NULL);
			SDL_Flip(screen);
			
			is_dirty = 0;
		}
	}
	
	SDL_FreeSurface(copy);
	SDL_FillRect(buffer, NULL, 0);
	SDL_Flip(screen);
	
	return status;
}