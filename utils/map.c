#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "map.h"

const char *field_symbol[] = {
	[FIELD_WALL] = "⌗", //⌗⏹
	[FIELD_NONE] = " ",
	[FIELD_FOOD] = "•", //•⏺
	[FIELD_HEAD] = "፠",
	[FIELD_BODY] = "፨",
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

Field map_getfield(Map *map, Coordinate coord) {
  if (coord.x < 0 || coord.x >= map->size || coord.y < 0 || coord.y >= map->size) {
    return FIELD_NONE;
  }
  
  return map->fields[coord.x][coord.y];
}

bool map_setfield(Map *map, Coordinate coord, Field field) {
  if (coord.x < 0 || coord.x >= map->size || coord.y < 0 || coord.y >= map->size) {
    return false;
  }

  map->fields[coord.x][coord.y] = field;
  
  return true;
}
