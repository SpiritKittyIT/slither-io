#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#include "memthread.h"
#include "../utils/shrmem.h"
#include "srvlogic.h"
#include "snakelist.h"
#include "../utils/socket.h"

void *memthread_start(void *arg) {
  MemthreadArgs *thread_args = arg;

  int turns_without_snakes = 0;
  bool game_over = false;
  int turns = 0;
  int turn_limit = shrmem_get_turn_limit(thread_args->map_state, TURN_MILISEC);

  while (!game_over) {
    printf("turns: %d\n", turns);
    printf("turns_without_snakes: %d\n", turns_without_snakes);
    if (game_turn(thread_args->map_state, thread_args->snake_list)) {
      turns_without_snakes = 0;
    }
    else {
      ++turns_without_snakes;
    }
    shrmem_inc_game_turns(thread_args->map_state);
    ++turns;

    if (turn_limit == 0 && turns_without_snakes >= INACTIVE_LIMIT) {
      game_over = true;
      printf("game_over a\n");
      continue;
    }

    if (turn_limit > 0 && turns > turn_limit) {
      printf("game_over b\n");
      game_over = true;
      continue;
    }

    usleep(TURN_MILISEC * 1000);
  }

  Message game_over_message;
  game_over_message.instruction = IST_GAME_OVER;
  send_message(thread_args->socfd, &game_over_message);

  return NULL;
}
