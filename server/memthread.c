#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>

#include "memthread.h"
#include "../utils/shrmem.h"
#include "srvlogic.h"
#include "snakelist.h"
#include "../utils/socket.h"

bool send_game_over(int socfd, int port) {
  Message message;

  message.pid = getpid();
  message.instruction = IST_GAME_OVER;

  struct sockaddr_in server_addr;
  if (!get_server_addr(port, &server_addr)) {
    return false;
  }

  printf("Sending game over message %d %d\n", socfd, message.pid);
  /*if (send_message(socfd, &server_addr, &message)) {
    return false;
  }*/

  return true;
}

void *memthread_start(void *arg) {
  MemthreadArgs *thread_args = arg;

  int turns_without_snakes = 0;
  bool game_over = false;
  int turns = 0;
  int turn_limit = shrmem_get_turn_limit(thread_args->shrmem, TURN_MILISEC);

  while (!game_over) {
    shrmem_print(thread_args->shrmem);
    if (game_turn(thread_args->shrmem, thread_args->snake_list)) {
      turns_without_snakes = 0;
    }
    else {
      ++turns_without_snakes;
    }
    shrmem_inc_game_turns(thread_args->shrmem);
    ++turns;

    if (turn_limit == 0 && turns_without_snakes >= INACTIVE_LIMIT) {
      game_over = true;
      continue;
    }

    if (turn_limit > 0 && turns > turn_limit) {
      game_over = true;
      continue;
    }

    usleep(TURN_MILISEC * 1000);
  }

  send_game_over(thread_args->socfd, thread_args->port);

  return NULL;
}
