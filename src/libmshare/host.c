#include <stdio.h>
#include <stdlib.h>
#include <msettings.h>
#include <signal.h>
#include <string.h>

// NOTE: simulating keymon

volatile sig_atomic_t quit = 0;
void term(int signum)
{
   QuitSettings();
   quit = 1;
}

void main(void) {
	struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
		
	InitSettings();
	SetBrightness(7);
	
	while (!quit);
}