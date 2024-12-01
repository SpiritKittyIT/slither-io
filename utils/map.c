#include <stdio.h>
#include <stdlib.h>

#include "map.h"

const char* ansi_code[] = {
	"\r",
	"\n",
	"\033[A",
	"\033[B",
	"\033[C",
	"\033[D",
	"\033[2J",
	"\033[H",
};

const char* field_symbol[] = {
	"⌗", //⌗⏹
	" ",
	"•", //•⏺
	"፠",
	"፨" 
};

Map *map_init(int size, int max_snakes) {
  Map *result = calloc(1, sizeof(Map));

  result->max_snakes = max_snakes;
  result->size = size + 2;
  result->snakes = 0;

  result->fields = calloc(result->size, sizeof(Field*));
  for (int i = 0; i < result->size; i++) {
      result->fields[i] = calloc(result->size, sizeof(Field));
  }

  Field field;
  for (int x = 0; x < result->size; x++)
  {
    for (int y = 0; y < result->size; y++)
    {
      field = FIELD_NONE;
      if (x == 0 || x == result->size - 1 || y == 0 || y == result->size - 1) {
        field = FIELD_WALL;
      }

      result->fields[x][y] = field;
    }
  }

  return result;
}

void map_destroy(Map *map) {
  for (int i = 0; i < map->size; i++) {
      free(map->fields[i]);
  }
  free(map->fields);
  free(map);
}

Field map_getfield(Map *map, int x, int y) {
  if (x < 0 || x >= map->size || y < 0 || y >= map->size) {
    return FIELD_NONE;
  }
  
  return map->fields[x][y];
}

void map_dispview(Map *map, int view_size, int head_x, int head_y) {
  int tlx = head_x - view_size / 2;
  int tly = head_y - view_size / 2;

  printf(ansi_code[ANSI_CURTL]);
  for (int x = tlx; x < tlx + view_size; x++)
  {
    for (int y = tly; y < tly + view_size; y++)
    {
      printf(field_symbol[map_getfield(map, x, y)]);
    }
    printf(ansi_code[ANSI_NEWLN]);
  }
}
