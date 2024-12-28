#pragma once

#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>

#include "map.h"
#include "snake.h"

#define MAX_CLIENTS 32
#define SHRMEM_FILE "/slither_shared_memory"
#define TURN_MILISEC 500

typedef struct {
  pid_t client_id;
  Coordinate coord;
  int score;
  bool alive;
} ClientHead;

typedef struct {
  ClientHead client_heads[MAX_CLIENTS];
  int clients;                   // Number of active clients
  int game_turns;                // Current game time
  int time_limit;                // Game time limit
  bool game_over;                // Game over flag
} GameInfo;

typedef struct {
  pthread_mutex_t mutex;         // Mutex for synchronizing access
  pthread_cond_t cond;           // Condition variable for signaling
  GameInfo game_info;            // Game information
  Map map;                       // Map instance
} Shrmem;

Shrmem *shrmem_create(pid_t pid, int map_size, int game_length, bool with_obstacles, const char *map_file);
Shrmem *shrmem_access(pid_t pid, size_t *size);
void shrmem_disconnect(Shrmem *shrmem, size_t size);
void shrmem_destroy(pid_t pid);
void shrmem_notify(Shrmem *shrmem);
Map *shrmem_get_map_init(Shrmem *shrmem);
GameInfo *shrmem_game_info_init(Shrmem *shrmem);
bool shrmem_get_update(Shrmem *shrmem, Map *map, GameInfo *game_info);
bool shrmem_get_client_head(Shrmem *shrmem, pid_t pid, ClientHead *client_head);
bool shrmem_add_client(Shrmem *shrmem, pid_t pid, Coordinate coord, int score);
Coordinate shrmem_get_spawn(Shrmem *shrmem);
bool shrmem_update_client(Shrmem *shrmem, int index, Coordinate coord, int score);
int shrmem_get_game_turns(Shrmem *shrmem);
void shrmem_inc_game_turns(Shrmem *shrmem);
int shrmem_get_turn_limit(Shrmem *shrmem, int turn_milisec);
void shrmem_print(Shrmem *shrmem);
