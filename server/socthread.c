#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "socthread.h"
#include "snakelist.h"
#include "../utils/socket.h"

static bool handle_message(SnakeList *snake_list, Message *message) {
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
      snakelist_remove(snake_list, message->pid);
      break;
    case IST_GAME_OVER:
      printf("Game over received %d.\n", message->pid);
      return true;
    default:
      printf("Unknown instruction %d from client %d\n", message->instruction, message->pid);
  }

  return false;
}

static void handle_new_connection(int server_socket, int *client_sockets) {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  int new_client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
  if (new_client_socket < 0) {
      perror("accept");
      return;
  }

  printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_sockets[i] == 0) {
      client_sockets[i] = new_client_socket;
      return;
    }
  }

  // If no free slot is found, reject the connection
  printf("Maximum clients reached. Rejecting connection.\n");
  close(new_client_socket);
}

static bool handle_client_messages(int *client_sockets, SnakeList *snake_list, fd_set *read_fds) {
  bool result = false;

  for (int i = 0; i < MAX_CLIENTS; i++) {
    int client_socket = client_sockets[i];
    if (client_socket > 0 && FD_ISSET(client_socket, read_fds)) {
      Message message;
      ssize_t bytes_received = recv(client_socket, &message, sizeof(Message), 0);

      if (bytes_received <= 0) {
        if (bytes_received == 0) {
          printf("Client disconnected on socket %d.\n", client_socket);
        } else {
          perror("recv");
        }
        close(client_socket);
        client_sockets[i] = 0;
        continue;
      }

      result = result ||handle_message(snake_list, &message);
    }
  }

  return result;
}

static void setup_fd_set(int server_socket, int *client_sockets, fd_set *read_fds, int *max_fd) {
  FD_ZERO(read_fds);
  FD_SET(server_socket, read_fds);
  *max_fd = server_socket;

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_sockets[i] > 0) {
      FD_SET(client_sockets[i], read_fds);
      if (client_sockets[i] > *max_fd) {
        *max_fd = client_sockets[i];
      }
    }
  }
}

void *socthread_start(void *args) {
  SocthreadArgs *thread_args = args;

  int server_socket = thread_args->socfd;
  SnakeList *snake_list = thread_args->snake_list;

  int client_sockets[MAX_CLIENTS] = {0};
  fd_set read_fds;
  int max_fd;

  printf("socthread started\n");
  while (true) {
    setup_fd_set(server_socket, client_sockets, &read_fds, &max_fd);

    int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
    if (activity < 0) {
      perror("select");
      break;
    }

    if (FD_ISSET(server_socket, &read_fds)) {
      handle_new_connection(server_socket, client_sockets);
    }

    if (handle_client_messages(client_sockets, snake_list, &read_fds)) {
      break;
    }
  }

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_sockets[i] > 0) {
      close(client_sockets[i]);
    }
  }
  printf("socthread end\n");

  return NULL;
}
