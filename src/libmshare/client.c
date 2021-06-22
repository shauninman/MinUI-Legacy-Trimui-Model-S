#include <stdio.h>
#include <stdlib.h>
#include <msettings.h>

#include <SDL/SDL.h>

// NOTE: simulating MinUI or libmmenu

int main(int argc , char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
	SDL_ShowCursor(0);
	
	InitSettings();
	
	printf("brightness: %i\n", GetBrightness());
	
	SDL_FillRect(screen, NULL, 0);
	SDL_Flip(screen);
		
	int quit = 0;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					quit = 1;
				break;
			}
		}
	}
	
	QuitSettings();
	SDL_Quit();
	return 0;
}