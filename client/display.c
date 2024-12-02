#include <stdio.h>

#include "../utils/map.h"
#include "winedit.h"
#include "display.h"

char *get_border(int interface_size, int x, int y) {
  if (x == 0) {
    if (y == 0) {
      return "┌─";
    }
    if (y == interface_size - 1) {
      return "┐";
    }
    return "──";
  }

  if (x == interface_size - 1) {
    if (y == 0) {
      return "└─";
    }
    if (y == interface_size - 1) {
      return "┘";
    }
    return "──";
  }

  if (y == 0) {
    return "│ ";
  }

  if (y == interface_size - 1) {
    return "│";
  }

  return NULL;
}

void map_dispview(Map *map, int view_size, int head_x, int head_y) {
  int tlx = head_x - view_size / 2;
  int tly = head_y - view_size / 2;

  int interface_size = view_size + 2;

  char *border = NULL;
  for (int x = 0; x < interface_size; x++)
  {
    move_cursor(x, 0);
    for (int y = 0; y < interface_size; y++)
    {
      border = get_border(interface_size, x, y);
      if (border == NULL) {
        printf("%s ", field_symbol[map_getfield(map, tlx + x, tly + y)]);
      }
      printf(border);
    }
  }
  fflush(stdout);
}

void print_instructions(int start_row, int start_col) {
  const char *instructions[] = {
    "Instructions:",
    "Press space to exit",
    "wasd or arrows to move",
  };
  int num_instructions = sizeof(instructions) / sizeof(instructions[0]);

  for (int i = 0; i < num_instructions; i++) {
    move_cursor(start_row + i, start_col);
    printf("%s", instructions[i]);
  }
  fflush(stdout);
}
