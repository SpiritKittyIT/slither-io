#pragma once

#include "map.h"

typedef enum {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
} Direction;

const Coordinate direction_coord[] = {
    [DIR_UP] = {0, 1},
    [DIR_DOWN] = {0, -1},
    [DIR_LEFT] = {-1, 0},
    [DIR_RIGHT] = {1, 0},
};

Coordinate coord_sum(Coordinate summand1, Coordinate summand2);

typedef struct {
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
