#include <stdio.h>

#include "../utils/map.h"
#include "../utils/snake.h"
#include "winedit.h"
#include "display.h"

char *get_border(int interface_size, Coordinate coord) {
  if (coord.x == 0) {
    if (coord.y == 0) {
      return "┌─";
    }
    if (coord.y == interface_size - 1) {
      return "┐";
    }
    return "──";
  }

  if (coord.x == interface_size - 1) {
    if (coord.y == 0) {
      return "└─";
    }
    if (coord.y == interface_size - 1) {
      return "┘";
    }
    return "──";
  }

  if (coord.y == 0) {
    return "│ ";
  }

  if (coord.y == interface_size - 1) {
    return "│";
  }

  return NULL;
}

void map_dispview(Map *map, int view_size, Coordinate coord) {
  int tlx = coord.x - view_size / 2;
  int tly = coord.y - view_size / 2;

  int interface_size = view_size + 2;

  Coordinate position = {.x = 0, .y = 0};
  char *border = NULL;
  for (int x = 0; x < interface_size; x++)
  {
    move_cursor(x, 0);
    for (int y = 0; y < interface_size; y++)
    {
      position.x = x;
      position.y = y;
      border = get_border(interface_size, position);
      if (border == NULL) {
        position.x += tlx;
        position.y += tly;
        printf("%s ", field_symbol[map_getfield(map, position)]);
      }
      printf(border);
    }
  }
  fflush(stdout);
}

static bool is_in_view(Coordinate head, int view_size, Coordinate coord) {
  int tlx = head.x - view_size / 2;
  int tly = head.y - view_size / 2;

  return coord.x >= tlx && coord.x <= tlx + view_size
      && coord.y >= tly && coord.y <= tly + view_size;
}

void display_snake(Snake *snake, int view_size) {
  BodyPart *body_part = snake->head;
  Coordinate head = {
    .x = body_part->coord.x - view_size / 2,
    .y = body_part->coord.y - view_size / 2
  };
  Coordinate coord = head;

  while (body_part != NULL)
  {
    move_cursor(coord.x, coord.y);
    printf("%s ", field_symbol[body_part->field]);
    body_part = body_part->next;
  }
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
