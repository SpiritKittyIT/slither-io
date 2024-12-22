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
	int max_snakes;
	int snakes;
	Field fields[];
} Map;

Map *map_new(int size, int max_snakes, bool with_obstacles, const char *from_file);
void map_destroy(Map *map);
Field map_getfield(Map *map, Coordinate coord);
bool map_setfield(Map *map, Coordinate coord, Field field);
bool add_food(Map *map);
void map_print(Map *map);
