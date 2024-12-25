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

static void display_scrmenu(int selected, bool paused, const char *srv_file) {
  clear_screen();
  for (int i = 0; i < 4; ++i) {
    if (i == 0 && !paused) {
      continue;
    }
    if (i == 1 && !srv_file) {
      continue;
    }

    if (i == selected) {
      printf("\033[7m%s\033[0m\n", scrmenu_options[i]);
    } else {
      printf("%s\n", scrmenu_options[i]);
    }
  }
}

static int next_valid_option(int current, int direction, bool paused, const char *srv_file) {
  const int optcount = 4;
  current = (current + direction + optcount) % optcount;
  while ((current == 0 && !paused) || (current == 1 && !srv_file)) {
    current = (current + direction + optcount) % optcount;
  }
  return current;
}

Screen open_scrmenu(bool *paused, const char *srv_file) {
  Screen screen = SCR_MENU;
  int selected = 0;

  if (!*paused) {
    selected = 1;
  }
  if (!srv_file && selected == 1) {
    selected = 2;
  }

  char c;
  while (screen == SCR_MENU) {
    display_scrmenu(selected, *paused, srv_file);

    c = getchar();
    switch (c) {
      case 'w': case 'W':
        selected = next_valid_option(selected, -1, *paused, srv_file);
        break;

      case 's': case 'S':
        selected = next_valid_option(selected, 1, *paused, srv_file);
        break;

      case '\n': case ' ':
        // Enter key
        switch (selected) {
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
  }

  return screen;
}
