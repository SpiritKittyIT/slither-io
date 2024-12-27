#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>

#include "screen.h"
#include "scrgame.h"
#include "winedit.h"
#include "display.h"

static void display_scrgame() {
  clear_screen();

  display_border();
}

Screen open_scrgame(bool *paused) {
  Screen screen = SCR_GAME;
  *paused = false;

	char c;
  while (screen == SCR_GAME) {
    display_scrgame();
    c = getchar();
    switch (c)
    {
      case 'w': case 'W':
        break;

      case 's': case 'S':
        break;

      case 'a': case 'A':
        break;

      case 'd': case 'D':
        break;

      case 'p': case 'P':
        *paused = true;
        screen = SCR_MENU;
        break;

      case 'q': case 'Q':
        screen = SCR_MENU;
        break;
      
      default:
        break;
    }
  }

	return screen;
}
