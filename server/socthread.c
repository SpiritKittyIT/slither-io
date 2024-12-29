#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "socthread.h"
#include "snakelist.h"
#include "../utils/socket.h"
#include "../utils/shrmem.h"
#include "srvlogic.h"
#include "snakelist.h"

static bool handle_message(SnakeList *snake_list, Shrmem *shrmem, Message *message) {
  printf("Received instruction %d from PID %d\n", message->instruction, message->pid);

  switch (message->instruction) {
    case IST_CONNECT:
      if (!snakelist_add(snake_list, message->pid)) {
        fprintf(stderr, "Failed to add new client.\n");
      }
      break;
    case IST_UP:
      snakelist_change_dir(snake_list, message->pid, DIR_UP);
      break;
    case IST_DOWN:
      snakelist_change_dir(snake_list, message->pid, DIR_DOWN);
      break;
    case IST_LEFT:
      snakelist_change_dir(snake_list, message->pid, DIR_LEFT);
      break;
    case IST_RIGHT:
      snakelist_change_dir(snake_list, message->pid, DIR_RIGHT);
      break;
    case IST_PAUSE:
      snakelist_pause(snake_list, message->pid);
      break;
    case IST_QUIT:
      int index = 0;
      if (snakelist_get_index(snake_list, message->pid, &index)) {
        srvlogic_snake_kill(shrmem, snake_list, index);
      }
      shrmem_remove_client(shrmem, message->pid);
      break;
    case IST_GAME_OVER:
      return true;
    default:
      printf("Unknown instruction %d from client %d\n", message->instruction, message->pid);
  }

  return false;
}

void *socthread_start(void *args) {
  SocthreadArgs *thread_args = args;
  Message message;

  // Receive messages in a loop
  while (true) {
    if (!receive_message(thread_args->socfd, &message)) {
      continue;
    }

    if (handle_message(thread_args->snake_list, thread_args->shrmem, &message)) {
      break;
    }
  }

  return NULL;
}
