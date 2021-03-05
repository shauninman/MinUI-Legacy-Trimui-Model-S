#ifndef menu_h__
#define menu_h__

#include <SDL/SDL.h>

typedef enum MenuReturnStatus {
	kStatusContinue = 0,
	kStatusSaveSlot = 1,
	kStatusLoadSlot = 11,
	kStatusOpenMenu = 23,
	kStatusExitGame = 31
} MenuReturnStatus;

typedef enum MenuReturnEvent {
	kMenuEventKeyDown = 0,
	kMenuEventKeyUp = 1, // use if emulated doesn't distinguish between down and just down, eg. race-od
} MenuReturnEvent;

typedef MenuReturnStatus (*ShowMenu_t)(char* rom_path, char* save_path_template, SDL_Surface* frame, MenuReturnEvent keyEvent);

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* frame, MenuReturnEvent keyEvent);
 
#endif  // menu_h__