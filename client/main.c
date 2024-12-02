#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "../utils/map.h"
#include "winedit.h"
#include "display.h"

int main() {
  struct termios orig_termios;
  init_game_screen(&orig_termios);

  Map *map = map_init(10, 1);
  int view_size = 12;

  // Initial display
  map_dispview(map, view_size, 5, 5);
  print_instructions(1, view_size * 2 + 5);

  usleep(5000000);
  map->fields[5][5] = FIELD_FOOD;
  map_dispview(map, view_size, 5, 5);

  char ch;
	while (1) {
		ch = getchar(); // Get a character without buffering

		if (ch == ' ') { // Check if space is pressed
			break;
		}
	}

  map_destroy(map);
  
  undo_game_screen(&orig_termios);

  return 0;
}
