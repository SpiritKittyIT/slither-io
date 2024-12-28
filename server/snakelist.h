#pragma once

#include <stdbool.h>
#include <pthread.h>

#include "sys/types.h"
#include "../utils/map.h"
#include "../utils/shrmem.h"

typedef struct {
  pthread_mutex_t mutex;
  Snake *snakes[MAX_CLIENTS];
  pid_t client_ids[MAX_CLIENTS];
  int snake_count;
} SnakeList;

SnakeList *snakelist_init();
void snakelist_destroy(SnakeList *snake_list);
bool snakelist_add(SnakeList *snake_list, pid_t client_id, Coordinate coord);
bool snakelist_spawn(SnakeList *snake_list, int index, Coordinate coord, Direction dir);
bool snakelist_pause(SnakeList *snake_list, int index);
pid_t snakelist_get_client_id(SnakeList *snake_list, int index);
Direction snakelist_get_dir(SnakeList *snake_list, int index);
bool snakelist_remove(SnakeList *snake_list, pid_t client_id);
bool snakelist_remove_i(SnakeList *snake_list, int index);
int snakelist_get_count(SnakeList *snake_list);
int snakelist_get_score(SnakeList *snake_list, int index);
Coordinate snakelist_get_head(SnakeList *snake_list, int index);
bool snakelist_change_dir(SnakeList *snake_list, pid_t client_id, Direction dir);
bool snakelist_leave(SnakeList *snake_list, pid_t client_id);
bool snakelist_left(SnakeList *snake_list, pid_t client_id);
