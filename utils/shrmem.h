#pragma once

#include <sys/types.h>

#include "map.h"
#include "snake.h"

#define MAX_CLIENTS 32
#define SHRMEM_FILE "/slither_shared_memory"

typedef struct {
  pid_t client_id;
  Coordinate coord;
  Direction dir;
} ClientHead;

typedef struct {
    pthread_mutex_t mutex;         // Mutex for synchronizing access
    pthread_cond_t cond;           // Condition variable for signaling
    Map map;                       // Map instance
    ClientHead client_heads[MAX_CLIENTS];
    int clients;                   // Number of active clients
} MapState;

MapState *shrmem_create(pid_t pid, int map_size, bool with_obstacles, const char *map_file);
MapState *shrmem_access(pid_t pid);
void shrmem_destroy(pid_t pid);
void shrmem_notify(MapState *state);
Map *shrmem_get_map_init(MapState *state);
bool shrmem_get_update(MapState *map_state, pid_t pid, Map *map, ClientHead *client_head);
