#include <stdio.h>
#include <stdlib.h>
#include <msettings.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

// NOTE: simulating MinUI or libmmenu

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

void render(SDL_Surface* screen, TTF_Font* font) {
	static SDL_Color color = {0xff,0xff,0xff};
	
	SDL_Surface* text;
	char buffer[16];
	
	SDL_FillRect(screen, NULL, 0);
	
	int brightness = GetBrightness();
	sprintf(buffer, "brightness: %i", brightness);
	text = TTF_RenderUTF8_Blended(font, buffer, color);
	SDL_BlitSurface(text, NULL, screen, &(SDL_Rect){16,16,0,0});
	SDL_FreeSurface(text);
	
	int volume = GetVolume();
	sprintf(buffer, "volume: %i", volume);
	text = TTF_RenderUTF8_Blended(font, buffer, color);
	SDL_BlitSurface(text, NULL, screen, &(SDL_Rect){16,32,0,0});
	SDL_FreeSurface(text);
	
	SDL_Flip(screen);
}

int main(int argc , char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
	SDL_ShowCursor(0);
	
	InitSettings();
	
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("/usr/res/BPreplayBold.otf", 16);
	
	render(screen, font);
	
	int quit = 0;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			SDLKey btn = event.key.keysym.sym;
			switch(event.type) {
				case SDL_KEYDOWN: {
					if (btn==TRIMUI_MENU) quit = 1;
				}
				break;
			}
		}
		
		render(screen, font);
	}
	
	TTF_CloseFont(font);
	QuitSettings();
	TTF_Quit();
	SDL_Quit();
	return 0;
}