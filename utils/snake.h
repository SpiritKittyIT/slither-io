#pragma once

#include "map.h"

typedef enum {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
} Direction;

extern const Coordinate direction_coord[];

Coordinate coord_sum(Coordinate summand1, Coordinate summand2);

typedef struct BodyPart {
  Field field;
  Coordinate coord;
  struct BodyPart *next;
} BodyPart;

typedef struct {
  BodyPart *head;
  BodyPart *tail;
  int size;
  Direction dir;
} Snake;

Snake *snake_init(Coordinate coord, Direction dir);
void snake_destroy(Snake *sll);
int snake_move(Snake *snake, Map *map);
bool snake_changedir(Snake *snake, Direction dir);
