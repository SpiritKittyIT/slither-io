#pragma once

#include <stdbool.h>
#include <sys/types.h>

#define SRVLIST_FILE "/slither_server_list"
#define SEM_FILE "/slither_server_list_sem"
#define MAX_SERVER_COUNT 256

typedef struct {
    pid_t pid;
    int socfd;
    int port;
} Server;

typedef struct {
    Server servers[MAX_SERVER_COUNT];
    int active_count;
} ServerList;

bool add_server_to_shared_memory(Server new_server);
bool remove_server_from_shared_memory(Server new_server);
bool get_active_server_list(ServerList *srvlist);
bool read_and_print_shared_memory();
