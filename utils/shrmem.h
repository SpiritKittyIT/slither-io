#pragma once

#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>

#include "map.h"
#include "snake.h"

#define MAX_CLIENTS 32
#define SHRMEM_FILE "/slither_shared_memory"

typedef struct {
  pid_t client_id;
  Coordinate coord;
  int score;
  bool alive;
} ClientHead;

typedef struct {
  pthread_mutex_t mutex;         // Mutex for synchronizing access
  pthread_cond_t cond;           // Condition variable for signaling
  Map map;                       // Map instance
  ClientHead client_heads[MAX_CLIENTS];
  int clients;                   // Number of active clients
  int game_turns;                 // Current game time
  int time_limit;                // Game time limit
} MapState;

MapState *shrmem_create(pid_t pid, int map_size, int game_length, bool with_obstacles, const char *map_file);
MapState *shrmem_access(pid_t pid);
void shrmem_destroy(pid_t pid);
void shrmem_notify(MapState *state);
Map *shrmem_get_map_init(MapState *state);
bool shrmem_get_update(MapState *map_state, pid_t pid, Map *map, ClientHead *client_head);
bool shrmem_get_client_head(MapState *state, pid_t pid, ClientHead *client_head);
bool shrmem_add_client(MapState *state, pid_t pid, Coordinate coord, int score);
Coordinate shrmem_get_spawn(MapState *state);
bool shrmem_update_client(MapState *state, int index, Coordinate coord, int score);
int shrmem_get_game_turns(MapState *state);
void shrmem_inc_game_turns(MapState *state);
int shrmem_get_turn_limit(MapState *state, int turn_milisec);
