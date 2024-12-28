#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>

#include "../utils/map.h"
#include "winedit.h"
#include "screen.h"
#include "scrmenu.h"
#include "scrcreate.h"
#include "scrgame.h"
#include "scrjoin.h"
#include "clientflags.h"

int main(int argc, char *argv[]) {
  const char *srv_file;
  const char *map_file;

	// Handle flags
	if (!clientflags_handle(argc, argv, &srv_file, &map_file)) {
    printf("srvfile: %d\n", !srv_file);
		return 1;
	}

  struct termios orig_termios;
  init_game_screen(&orig_termios);

  bool paused = false;
  pid_t srv_pid = -1;

  Screen screen = SCR_MENU;
  
  while (screen != SCR_QUIT) {
    switch (screen)
    {
    case SCR_MENU:
      screen = open_scrmenu(&paused, srv_file);
      break;

    case SCR_CREATE:
      screen = open_scrcreate(&paused, &srv_pid, srv_file, map_file);
      break;

    case SCR_GAME:
      screen = open_scrgame(&paused, srv_pid);
      break;

    case SCR_JOIN:
      screen = open_scrjoin(&paused, &srv_pid);
      break;
    
    default:
      break;
    }
  }

  // a very significant change done for the sake of a mandatory merge
  
  undo_game_screen(&orig_termios);

  return 0;
}
