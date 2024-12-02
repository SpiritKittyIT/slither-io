#pragma once

typedef enum {
	FIELD_WALL,
	FIELD_NONE,
	FIELD_FOOD,
	FIELD_HEAD,
	FIELD_BODY,
} Field;

extern const char *field_symbol[];

typedef struct Map
{
	int size;
	int max_snakes;
	int snakes;
	Field **fields;
} Map;

Map *map_init(int size, int max_snakes);
void map_destroy(Map *map);
Field map_getfield(Map *map, int x, int y);
