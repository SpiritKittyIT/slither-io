#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "../utils/map.h"
#include "winedit.h"
#include "display.h"
#include "screen.h"
#include "scrmenu.h"
#include "scrcreate.h"

int main() {
  struct termios orig_termios;
  init_game_screen(&orig_termios);

  Screen screen = SCR_MENU;
  
  while (screen != SCR_QUIT) {
    switch (screen)
    {
    case SCR_MENU:
      screen = open_scrmenu(false);
      break;

    case SCR_CREATE:
      screen = open_scrcreate();
      break;

    case SCR_GAME:
      screen = SCR_QUIT;
      break;

    case SCR_JOIN:
      screen = SCR_QUIT;
      break;
    
    default:
      break;
    }
  }
  
  undo_game_screen(&orig_termios);

  return 0;
}
