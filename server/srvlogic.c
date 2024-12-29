#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

#include "srvlogic.h"
#include "../utils/snake.h"
#include "../utils/map.h"
#include "../utils/shrmem.h"
#include "snakelist.h"

static bool srvlogic_snake_move(Shrmem *shrmem, SnakeList *snake_list, int index) {
  pthread_mutex_lock(&shrmem->mutex);
  pthread_mutex_lock(&snake_list->mutex);
  bool result = snake_move(snake_list->snakes[index], &shrmem->map);
  pthread_mutex_unlock(&snake_list->mutex);
  pthread_mutex_unlock(&shrmem->mutex);

  return result;
}

bool srvlogic_snake_kill(Shrmem *shrmem, SnakeList *snake_list, int index) {
  pthread_mutex_lock(&shrmem->mutex);
  pthread_mutex_lock(&snake_list->mutex);
  
  shrmem->game_info.client_heads[index].alive = false;
  BodyPart *body_part = snake_list->snakes[index]->head;
  while (body_part != NULL) {
    map_setfield(&shrmem->map, body_part->coord, FIELD_NONE);
    body_part = body_part->next;
  }

  pthread_mutex_unlock(&snake_list->mutex);
  pthread_mutex_unlock(&shrmem->mutex);

  snakelist_remove_i(snake_list, index);

  return true;
}

bool game_turn(Shrmem *shrmem, SnakeList *snake_list) {
  ClientHead client_head;

  int snake_count = snakelist_get_count(snake_list);
  int i = 0;
  while (i < snake_count) {
    pid_t client_id = snakelist_get_client_id(snake_list, i);

    if (!shrmem_get_client_head(shrmem, client_id, &client_head)) { // new client
      Coordinate coord = shrmem_get_spawn(shrmem);
      shrmem_add_client(shrmem, client_id, coord, 1);
      shrmem_print(shrmem);
      snake_list->snakes[i] = snake_init(coord, DIR_NONE);

      ++i;
      continue;
    }

    if (snakelist_paused_i(snake_list, i)) {
      ++i;
      continue;
    }

    if (!srvlogic_snake_move(shrmem, snake_list, i)) { // snake died
      srvlogic_snake_kill(shrmem, snake_list, i);
      snake_count = snakelist_get_count(snake_list);

      continue;
    }

    client_head.score = snakelist_get_score(snake_list, i);
    client_head.coord = snakelist_get_head(snake_list, i);
    shrmem_update_client(shrmem, &client_head);
    ++i;
  }

  shrmem_spawn_food(shrmem, snake_count);

  shrmem_notify(shrmem);

  return snake_count > 0;
}
