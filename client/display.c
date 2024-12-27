#include <stdio.h>

#include "../utils/map.h"
#include "../utils/snake.h"
#include "winedit.h"
#include "display.h"

#define VIEW_SIZE 10

static char *get_border(int border_size, Coordinate coord) {
  int border_edge = border_size - 1;

  if (coord.x == 0) {
    if (coord.y == 0) {
      return "┌─";
    }
    if (coord.y == border_edge) {
      return "┐";
    }
    return "──";
  }

  if (coord.x == border_edge) {
    if (coord.y == 0) {
      return "└─";
    }
    if (coord.y == border_edge) {
      return "┘";
    }
    return "──";
  }

  if (coord.y == 0) {
    return "│ ";
  }

  if (coord.y == border_edge) {
    return "│";
  }

  return "  ";
}

void display_border() {
  int border_size = VIEW_SIZE + 2;
  Coordinate coord = {.x = 0, .y = 0};
  char *symbol = NULL;

  for (int x = 0; x < border_size; x++)
  {
    move_cursor(x, 0);
    for (int y = 0; y < border_size; y++)
    {
      coord.x = x;
      coord.y = y;
      symbol = get_border(border_size, coord);
      printf(symbol);
    }
  }
  fflush(stdout);
}

/*void map_dispview(Map *map, int view_size, Coordinate coord) {
  int tlx = coord.x - view_size / 2;
  int tly = coord.y - view_size / 2;

  int interface_size = view_size + 2;

  Coordinate position = {.x = 0, .y = 0};
  char *border = NULL;
  for (int x = 0; x < interface_size; x++)
  {
    move_cursor(x, 2);
    for (int y = 0; y < interface_size; y++)
    {
      position.x = x;
      position.y = y;
      border = get_border(position);
      if (border == NULL) {
        position.x += tlx;
        position.y += tly;
        printf("%s ", field_symbol[map_getfield(map, position)]);
      }
      printf(border);
    }
  }
  fflush(stdout);
}*/

void print_instructions(int start_row, int start_col) {
  const char *instructions[] = {
    "Instructions:",
    "Press Q to exit",
    "Press P to pause",
    "wasd or arrows to move",
  };
  int num_instructions = sizeof(instructions) / sizeof(instructions[0]);

  for (int i = 0; i < num_instructions; i++) {
    move_cursor(start_row + i, start_col);
    printf("%s", instructions[i]);
  }
  fflush(stdout);
}
