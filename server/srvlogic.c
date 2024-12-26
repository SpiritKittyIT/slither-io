#include <stdbool.h>
#include <pthread.h>

#include "srvlogic.h"
#include "../utils/snake.h"
#include "../utils/map.h"
#include "../utils/shrmem.h"
#include "snakelist.h"

static bool srvlogic_snake_move(MapState *map_state, SnakeList *snake_list, int index) {
  pthread_mutex_lock(&map_state->mutex);
  pthread_mutex_lock(&snake_list->mutex);
  bool result = snake_move(snake_list->snakes[index], &map_state->map);
  pthread_mutex_unlock(&snake_list->mutex);
  pthread_mutex_unlock(&map_state->mutex);

  return result;
}

static bool srvlogic_snake_kill(MapState *map_state, SnakeList *snake_list, int index) {
  pthread_mutex_lock(&map_state->mutex);
  pthread_mutex_lock(&snake_list->mutex);
  
  map_state->client_heads[index].alive = false;
  BodyPart *body_part = snake_list->snakes[index]->head;
  while (body_part != NULL) {
    map_setfield(&map_state->map, body_part->coord, FIELD_NONE);
    body_part = body_part->next;
  }

  pthread_mutex_unlock(&snake_list->mutex);
  pthread_mutex_unlock(&map_state->mutex);

  snakelist_remove_i(snake_list, index);

  return true;
}

bool game_turn(MapState *map_state, SnakeList *snake_list) {
  ClientHead client_head;

  int snake_count = snakelist_get_count(snake_list);
  int i = 0;
  while (i < snake_count) {
    pid_t client_id = snakelist_get_client_id(snake_list, i);

    if (!shrmem_get_client_head(map_state, client_id, &client_head)) { // new client
      Coordinate coord = shrmem_get_spawn(map_state);
      shrmem_add_client(map_state, client_id, coord, 1);

      ++i;
      continue;
    }

    if (!srvlogic_snake_move(map_state, snake_list, i)) { // snake died
      srvlogic_snake_kill(map_state, snake_list, i);
      snake_count = snakelist_get_count(snake_list);

      ++i;
      continue;
    }

    int score = snakelist_get_score(snake_list, i);
    Coordinate head = snakelist_get_head(snake_list, i);
    shrmem_update_client(map_state, client_id, head, score);
    ++i;
  }

  shrmem_notify(map_state);

  return snake_count > 0;
}
