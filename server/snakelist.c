#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "snakelist.h"
#include "../utils/snake.h"
#include "../utils/shrmem.h"

SnakeList *snakelist_init() {
  SnakeList *snake_list = calloc(1, sizeof(SnakeList));
  snake_list->snake_count = 0;
  pthread_mutex_init(&snake_list->mutex, NULL);

  return snake_list;
}

void snakelist_destroy(SnakeList *snake_list) {
  for (int i = 0; i < snake_list->snake_count; ++i) {
    snake_destroy(snake_list->snakes[i]);
  }
  pthread_mutex_destroy(&snake_list->mutex);
  free(snake_list);
}

bool snakelist_add(SnakeList *snake_list, pid_t client_id, Coordinate coord) {
  pthread_mutex_lock(&snake_list->mutex);

  if (snake_list->snake_count >= MAX_CLIENTS) {
    pthread_mutex_unlock(&snake_list->mutex);
    return false;
  }

  snake_list->client_ids[snake_list->snake_count] = client_id;
  snake_list->snakes[snake_list->snake_count] = snake_init(coord, DIR_UP);

  printf("added snake: %d\n", snake_list->snakes[snake_list->snake_count]->dir);
  ++snake_list->snake_count;

  pthread_mutex_unlock(&snake_list->mutex);

  return true;
}

bool snakelist_spawn(SnakeList *snake_list, int index, Coordinate coord, Direction dir) {
  pthread_mutex_lock(&snake_list->mutex);
  snake_list->snakes[index] = snake_init(coord, dir);
  pthread_mutex_unlock(&snake_list->mutex);

  return true;
}

bool snakelist_pause(SnakeList *snake_list, int index) {
  pthread_mutex_lock(&snake_list->mutex);
  snake_list->snakes[index]->paused = true;
  pthread_mutex_unlock(&snake_list->mutex);

  return true;
}

pid_t snakelist_get_client_id(SnakeList *snake_list, int index) {
  pid_t result = 0;

  pthread_mutex_lock(&snake_list->mutex);
  result = snake_list->client_ids[index];
  pthread_mutex_unlock(&snake_list->mutex);

  return result;
}

Direction snakelist_get_dir(SnakeList *snake_list, int index) {
  Direction result = DIR_UP;

  pthread_mutex_lock(&snake_list->mutex);
  result = snake_list->snakes[index]->dir;
  pthread_mutex_unlock(&snake_list->mutex);  

  return result;
}

bool snakelist_remove(SnakeList *snake_list, pid_t client_id) {
  pthread_mutex_lock(&snake_list->mutex);

  for (int i = 0; i < snake_list->snake_count; ++i) {
    if (snake_list->client_ids[i] == client_id) {
      snake_destroy(snake_list->snakes[i]);
      --snake_list->snake_count;

      snake_list[i] = snake_list[snake_list->snake_count];
      snake_list->snakes[snake_list->snake_count] = NULL;

      snake_list->client_ids[i] = snake_list->client_ids[snake_list->snake_count];
      snake_list->client_ids[snake_list->snake_count] = 0;

      pthread_mutex_unlock(&snake_list->mutex);
      return true;
    }
  }

  pthread_mutex_unlock(&snake_list->mutex);
  return false;
}

bool snakelist_remove_i(SnakeList *snake_list, int index) {
  pthread_mutex_lock(&snake_list->mutex);

  snake_destroy(snake_list->snakes[index]);
  --snake_list->snake_count;

  snake_list[index] = snake_list[snake_list->snake_count];
  snake_list->snakes[snake_list->snake_count] = NULL;

  snake_list->client_ids[index] = snake_list->client_ids[snake_list->snake_count];
  snake_list->client_ids[snake_list->snake_count] = 0;

  pthread_mutex_unlock(&snake_list->mutex);
  return true;
}

int snakelist_get_count(SnakeList *snake_list) {
  int result = 0;
  pthread_mutex_lock(&snake_list->mutex);
  result = snake_list->snake_count;
  pthread_mutex_unlock(&snake_list->mutex);

  return result;
}

int snakelist_get_score(SnakeList *snake_list, int index) {
  int result = 1;
  pthread_mutex_lock(&snake_list->mutex);
  result = snake_list->snakes[index]->size;
  pthread_mutex_unlock(&snake_list->mutex);

  return result;
}

Coordinate snakelist_get_head(SnakeList *snake_list, int index) {
  Coordinate result;
  pthread_mutex_lock(&snake_list->mutex);
  result = snake_list->snakes[index]->head->coord;
  pthread_mutex_unlock(&snake_list->mutex);

  return result;
}

bool snakelist_change_dir(SnakeList *snake_list, pid_t client_id, Direction dir) {
  pthread_mutex_lock(&snake_list->mutex);

  for (int i = 0; i < snake_list->snake_count; ++i) {
    if (snake_list->client_ids[i] == client_id) {
      snake_changedir(snake_list->snakes[i], dir);
      pthread_mutex_unlock(&snake_list->mutex);
      return true;
    }
  }

  pthread_mutex_unlock(&snake_list->mutex);
  return false;
}

bool snakelist_leave(SnakeList *snake_list, pid_t client_id) {
  pthread_mutex_lock(&snake_list->mutex);

  for (int i = 0; i < snake_list->snake_count; ++i) {
    if (snake_list->client_ids[i] == client_id) {
      snake_list->snakes[i]->head->field = FIELD_NONE;
      pthread_mutex_unlock(&snake_list->mutex);
      return true;
    }
  }

  pthread_mutex_unlock(&snake_list->mutex);
  return false;
}

bool snakelist_left(SnakeList *snake_list, pid_t client_id) {
  pthread_mutex_lock(&snake_list->mutex);

  bool result = false;

  for (int i = 0; i < snake_list->snake_count; ++i) {
    if (snake_list->client_ids[i] == client_id) {
      result = snake_list->snakes[i]->head->field == FIELD_NONE;
      break;
    }
  }

  pthread_mutex_unlock(&snake_list->mutex);
  return result;
}
