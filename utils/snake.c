#include <string.h>
#include <stdlib.h>

#include "sll.h"
#include "map.h"
#include "snake.h"

static BodyPart *snake_create_bodypart(Coordinate coord, Field field) {
  BodyPart *body_part = calloc(1, sizeof(BodyPart));
  body_part->coord = coord;
  body_part->field = field;
  body_part->next = NULL;

  return body_part;
}

static void snake_grow(Snake *snake, Coordinate coord) {
  snake->tail->next = snake_create_bodypart(coord, FIELD_BODY);
  snake->tail = snake->tail->next;
  ++snake->size;
}

Coordinate coord_sum(Coordinate summand1, Coordinate summand2) {
  Coordinate result = {.x = summand1.x + summand2.x, .y = summand1.y + summand2.y};
  return result;
}

Snake *snake_init(Coordinate coord, Direction dir) {
  Snake *snake = calloc(1, sizeof(snake));

  snake->head = snake_create_bodypart(coord, FIELD_HEAD);
  snake->tail = snake->head;
  snake->dir = dir;
  snake->size = 1;
  
  return snake;
}

void snake_destroy(Snake *snake) {
  while (snake->head != NULL)
  {
    BodyPart *next_bp = snake->head->next;
    free(snake->head);
    snake->head = next_bp;
  }

  free(snake);
}

int snake_move(Snake *snake, Map *map) {
  BodyPart *body_part = snake->head;
  Coordinate coord = coord_sum(body_part->coord, direction_coord[snake->dir]);
  Field field = map_getfield(map, coord);

  while (body_part != NULL) {
    Coordinate tmp_coord = body_part->coord;
    body_part->coord = coord;
    coord = tmp_coord;
  }

  if (field == FIELD_FOOD) {
    map_setfield(map, coord, FIELD_NONE);
    snake_grow(snake, coord);
  }

  return 0;
}

bool snake_changedir(Snake *snake, Direction dir) {
  if (snake->dir == dir) {
    return false;
  }

  if (snake->dir == DIR_UP && dir == DIR_DOWN)
  {
    return false;
  }

  if (snake->dir == DIR_DOWN && dir == DIR_UP)
  {
    return false;
  }

  if (snake->dir == DIR_LEFT && dir == DIR_RIGHT)
  {
    return false;
  }

  if (snake->dir == DIR_RIGHT && dir == DIR_LEFT)
  {
    return false;
  }

  snake->dir = dir;
  
  return true;
}
