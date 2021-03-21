#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <fcntl.h>
#include <unistd.h>

enum {
	TRIMUI_A = SDLK_SPACE,
	TRIMUI_B = SDLK_LCTRL,
};

int main(int argc , char* argv[]) {
	if (argc<2) {
		puts("Usage: confirm image");
		puts("  then check for a file named OKAY");
		return 0;
	}
	
	char path[256];
	strncpy(path,argv[1],256);
	
	if (SDL_Init(SDL_INIT_VIDEO)==-1) {
		puts("could not init SDL");
		puts(SDL_GetError());
		
	}
	
	SDL_Surface* screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
	SDL_ShowCursor(0);
	
	SDL_Surface *image = IMG_Load(path);
	if (image==NULL) puts(IMG_GetError());
	SDL_BlitSurface(image, NULL, screen, NULL);
	SDL_Flip(screen);
	
	unlink("OKAY");
	
	int quit = 0;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN: {
					SDLKey btn = event.key.keysym.sym;
					if (btn==TRIMUI_A) {
						close(open("OKAY", O_RDWR|O_CREAT, 0777)); // basically touch
						quit = 1;
					}
					else if (btn==TRIMUI_B) {
						quit = 1;
					}
				} break;
			}
		}
	}
	SDL_FillRect(screen, NULL, 0);
	SDL_Flip(screen);
	
	SDL_FreeSurface(image);
	IMG_Quit();
	SDL_Quit();
	return 0;
}