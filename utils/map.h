#pragma once

#include <stdbool.h>

#define OBSTACLE_PERCENT 20

typedef enum {
	FIELD_WALL,
	FIELD_NONE,
	FIELD_FOOD,
	FIELD_HEAD,
	FIELD_BODY,
} Field;

extern const char *field_symbol[];

typedef struct {
  int x;
  int y;
} Coordinate;

typedef struct {
	int size;
	size_t fields_size;
	Field fields[];
} Map;

Map *map_new(int size, bool with_obstacles, const char *from_file);
void map_destroy(Map *map);
Field map_getfield(Map *map, Coordinate coord);
bool map_setfield(Map *map, Coordinate coord, Field field);
bool spawn_food(Map *map, int food_count);
Coordinate get_spawn_location(Map *map);
void map_print(Map *map);
