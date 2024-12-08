#include <stdio.h>
#include <stdbool.h>

#include "screen.h"
#include "scrmenu.h"
#include "winedit.h"

static const char* scrmenu_options[] = {
  "Resume Game",
  "New Game",
  "Join Game",
  "Quit"
};

static void display_scrmenu(int selected, bool paused) {
  clear_screen();
  for (int i = 0; i < 4; ++i) {
    if (i == 0 && !paused) {
      continue;
    }

    if (i == selected) {
      printf("\033[7m%s\033[0m\n", scrmenu_options[i]);
    } else {
      printf("%s\n", scrmenu_options[i]);
    }
  }
}

Screen open_scrmenu(const bool paused) {
  Screen screen = SCR_MENU;
  int selected = paused ? 0 : 1;
  const int optcount = 4;

  display_scrmenu(selected, paused);

	char c;
  while (screen == SCR_MENU) {
    c = getchar();
    switch (c)
    {
      case 'w': case 'W':
        selected = (selected - 1 + optcount) % optcount; // Wrap around
        if (!paused && selected == 0) {
          selected = 3;
        }
        break;

      case 's': case 'S':
        selected = (selected + 1) % optcount; // Wrap around
        if (!paused && selected == 0) {
          selected = 1;
        }
        break;

      case '\n': case ' ':
        // Enter key
        switch (selected)
        {
          case 0:
            screen = SCR_GAME;
            break;
          case 1:
            screen = SCR_CREATE;
            break;
          case 2:
            screen = SCR_JOIN;
            break;
          default:
            screen = SCR_QUIT;
            break;
        }
        break;
      
      default:
        break;
    }
    display_scrmenu(selected, paused);
  }

	return screen;
}
