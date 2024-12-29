#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <netdb.h>

#include "screen.h"
#include "scrgame.h"
#include "winedit.h"
#include "dispthread.h"
#include "../utils/socket.h"
#include "../utils/srvlist.h"

/*static int asdf() {
    // Step 2: Configure the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345); // Replace with the server's bound port
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Use localhost (127.0.0.1)

    return 0;
}*/

static void get_server(Server *server) {
  ServerList *srvlist = calloc(1, sizeof(ServerList));
  if (!srvlist) {
    return;
  }

  if (!get_active_server_list(srvlist)) {
    free(srvlist);

    return;
  }

  for (int i = 0; i < srvlist->active_count; i++) {
    if (srvlist->servers[i].pid == server->pid) {
      server->port = srvlist->servers[i].port;
    }
  }

  free(srvlist);
}

Screen open_scrgame(bool *paused, Server *server) {
  if (server->pid == 0) {
    return SCR_MENU;
  }

  if (server->port == 0) {
    get_server(server);
  }

  int socfd;
  if (!bind_client_socket(&socfd)) {
    return SCR_MENU;
  }

  struct sockaddr_in server_addr;
  get_server_addr(server->port, &server_addr);

  Message message;
  message.pid = getpid();
  if (*paused) {
    *paused = false;
    message.instruction = IST_PAUSE;
    send_message(socfd, &server_addr, &message);
  }
  else {
    message.instruction = IST_CONNECT;
    send_message(socfd, &server_addr, &message);
  }

  Screen screen = SCR_GAME;

  DispthreadArgs thread_args;
  thread_args.client_pid = getpid();
  thread_args.server = server;
  thread_args.paused = paused;
  pthread_t dispthread;
  pthread_create(&dispthread, NULL, &start_dispthread, &thread_args);

	char c;
  while (screen == SCR_GAME) {
    c = getchar();
    switch (c)
    {
      case 'w': case 'W':
        message.instruction = IST_UP;
        send_message(socfd, &server_addr, &message);
        break;

      case 's': case 'S':
        message.instruction = IST_DOWN;
        send_message(socfd, &server_addr, &message);
        break;

      case 'a': case 'A':
        message.instruction = IST_LEFT;
        send_message(socfd, &server_addr, &message);
        break;

      case 'd': case 'D':
        message.instruction = IST_RIGHT;
        send_message(socfd, &server_addr, &message);
        break;

      case 'p': case 'P':
        message.instruction = IST_PAUSE;
        send_message(socfd, &server_addr, &message);
        *paused = true;
        screen = SCR_MENU;
        break;

      case 'q': case 'Q':
        message.instruction = IST_QUIT;
        send_message(socfd, &server_addr, &message);
        server->pid = 0;
        server->port = 0;
        screen = SCR_MENU;
        break;
      
      default:
        break;
    }
  }

  unbind_socket(&socfd);
  pthread_join(dispthread, NULL);

	return screen;
}
