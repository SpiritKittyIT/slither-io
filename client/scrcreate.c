#include <stdio.h>
#include <stdbool.h>

#include "screen.h"
#include "scrcreate.h"
#include "winedit.h"

static const char* scrcreate_options[] = {
  "Return to Menu",
  "Create Game",
  "World type",
  "Game length",
};

static const char* scrcreate_world_types[] = {
  "Empty",
  "Walls",
};

static const char* scrcreate_game_lengths[] = {
  "infinite",
  "10 minutes",
  "20 minutes",
  "30 minutes",
};

static void display_scrcreate(int se_count, int selected, int selected_wt, int selected_gl) {
  clear_screen();
  for (int i = 0; i < se_count; ++i) {
    switch (i)
    {
      case 2:
        if (i == selected) {
          printf("\033[7m%s: %s\033[0m\n", scrcreate_options[i], scrcreate_world_types[selected_wt]);
        } else {
          printf("%s %s\n", scrcreate_options[i], scrcreate_world_types[selected_wt]);
        }
        break;
      case 3:
        if (i == selected) {
          printf("\033[7m%s: %s\033[0m\n", scrcreate_options[i], scrcreate_game_lengths[selected_gl]);
        } else {
          printf("%s %s\n", scrcreate_options[i], scrcreate_game_lengths[selected_gl]);
        }
        break;
      default:
        if (i == selected) {
          printf("\033[7m%s\033[0m\n", scrcreate_options[i]);
        } else {
          printf("%s\n", scrcreate_options[i]);
        }
        break;
    }
  }
}

Screen open_scrcreate() {
  Screen screen = SCR_CREATE;
  int selected = 0;
  int selected_wt = 0;
  int selected_gl = 0;
  const int se_count = 4;
  const int wt_count = 2;
  const int gl_count = 4;

  display_scrcreate(se_count, selected, selected_wt, selected_gl);

	char c;
  while (screen == SCR_CREATE) {
    c = getchar();
    switch (c)
    {
      case 'w': case 'W':
        selected = (selected - 1 + se_count) % se_count; // Wrap around
        break;

      case 's': case 'S':
        selected = (selected + 1) % se_count; // Wrap around
        break; 

      case 'a': case 'A':
        switch (selected)
        {
          case 2:
            selected_wt = (selected_wt - 1 + wt_count) % wt_count; // Wrap around
            break;
          case 3:
            selected_gl = (selected_gl - 1 + gl_count) % gl_count; // Wrap around
            break;
          default:
            break;
        }
        break;

      case 'd': case 'D':
        switch (selected)
        {
          case 2:
            selected_wt = (selected_wt + 1) % wt_count; // Wrap around
            break;
          case 3:
            selected_gl = (selected_gl + 1) % gl_count; // Wrap around
            break;
          
          default:
            break;
        }
        break;

      case '\n': case ' ':
        switch (selected)
        {
          case 0:
            screen = SCR_MENU;
            break;
          case 1:
            screen = SCR_GAME;
            break;
          default:
            break;
        }
        break;

      default:
        break;
    }
    display_scrcreate(se_count, selected, selected_wt, selected_gl);
  }

	return screen;
}
