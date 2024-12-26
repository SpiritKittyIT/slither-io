#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "shrmem.h"
#include "map.h"
#include "snake.h"

// Function to create shared memory and initialize MapState
MapState *shrmem_create(pid_t pid, int map_size, int game_length, bool with_obstacles, const char *map_file) {
  char shm_path[256];
  snprintf(shm_path, sizeof(shm_path), "%s_%d", SHRMEM_FILE, pid);

  int fd = shm_open(shm_path, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    perror("shm_open");
    close(fd);
    return NULL;
  }

  Map *temp_map = map_new(map_size, with_obstacles, map_file);
  if (!temp_map) {
		fprintf(stderr, "Failed to create map\n");
    close(fd);
    return NULL;
	}
  size_t struct_size = sizeof(MapState) + temp_map->fields_size;

  if (ftruncate(fd, struct_size) == -1) {
    perror("ftruncate");
    close(fd);
    free(temp_map);
    return NULL;
  }

  MapState *state = mmap(NULL, struct_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (state == MAP_FAILED) {
    perror("mmap");
    close(fd);
    free(temp_map);
    return NULL;
  }

  close(fd);

  // Initialize synchronization primitives and MapState
  pthread_mutexattr_t mutex_attr;
  pthread_condattr_t cond_attr;

  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);

  pthread_condattr_init(&cond_attr);
  pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);

  pthread_mutex_init(&state->mutex, &mutex_attr);
  pthread_cond_init(&state->cond, &cond_attr);

  // Initialize the map in shared memory
  memcpy(&state->map, temp_map, temp_map->fields_size);
  free(temp_map);

  state->clients = 0;
  state->game_turns = 0;
  state->time_limit = game_length;

  pthread_mutexattr_destroy(&mutex_attr);
  pthread_condattr_destroy(&cond_attr);

  return state;
}

// Cleanup function to destroy shared memory and synchronization primitives
void shrmem_destroy(pid_t pid) {
  char shm_path[256];
  snprintf(shm_path, sizeof(shm_path), "%s_%d", SHRMEM_FILE, pid);

  if (shm_unlink(shm_path) == -1) {
    perror("shm_unlink");
  }
}

// Function to access shared memory given a PID
MapState *shrmem_access(pid_t pid) {
  char shm_path[256];
  snprintf(shm_path, sizeof(shm_path), "%s_%d", SHRMEM_FILE, pid);

  int fd = shm_open(shm_path, O_RDWR, 0666);
  if (fd == -1) {
    perror("shm_open");
    return NULL;
  }

  struct stat shm_stat;
  if (fstat(fd, &shm_stat) == -1) {
    perror("fstat");
    close(fd);
    return NULL;
  }

  MapState *state = mmap(NULL, shm_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (state == MAP_FAILED) {
    perror("mmap");
    close(fd);
    return NULL;
  }

  close(fd);
  return state;
}

// Function to notify waiting processes
void shrmem_notify(MapState *state) {
  pthread_mutex_lock(&state->mutex);
  pthread_cond_broadcast(&state->cond);
  pthread_mutex_unlock(&state->mutex);
}

Map *shrmem_get_map_init(MapState *state) {
  Map *result;
  pthread_mutex_lock(&state->mutex);
  result = calloc(1, sizeof(Map) + state->map.fields_size);
  result->size = state->map.size;
  result->fields_size = state->map.fields_size;
  memcpy(result->fields, state->map.fields, state->map.fields_size);
  pthread_mutex_unlock(&state->mutex);

  return result;
}

bool shrmem_get_update(MapState *state, pid_t pid, Map *map, ClientHead *client_head) {
  bool found = false;

  pthread_mutex_lock(&state->mutex);
  pthread_cond_wait(&state->cond, &state->mutex);

  // Copy fields from mapstate
  memcpy(map->fields, state->map.fields, state->map.fields_size);

  // Find the ClientHead that matches the provided pid
  for (int i = 0; i < state->clients; i++) {
    if (state->client_heads[i].client_id == pid) {
      *client_head = state->client_heads[i];  // Direct structure copy
      found = true;
      break;
    }
  }

  pthread_mutex_unlock(&state->mutex);

  return found;
}

bool shrmem_get_client_head(MapState *state, pid_t pid, ClientHead *client_head) {
  bool found = false;

  pthread_mutex_lock(&state->mutex);
  for (int i = 0; i < state->clients; i++) {
    if (state->client_heads[i].client_id == pid) {
      *client_head = state->client_heads[i];  // Direct structure copy
      found = true;
      break;
    }
  }
  pthread_mutex_unlock(&state->mutex);

  return found;
}

bool shrmem_add_client(MapState *state, pid_t pid, Coordinate coord, int score) {
  pthread_mutex_lock(&state->mutex);

  if (state->clients < MAX_CLIENTS) {
    state->client_heads[state->clients].client_id = pid;
    state->client_heads[state->clients].coord = coord;
    state->client_heads[state->clients].score = score;
    state->client_heads[state->clients].alive = true;
    state->clients++;

    pthread_mutex_unlock(&state->mutex);

    return true;
  }

  pthread_mutex_unlock(&state->mutex);

  return false;
}

Coordinate shrmem_get_spawn(MapState *state) {
  pthread_mutex_lock(&state->mutex);
  Coordinate result = get_spawn_location(&state->map);
  pthread_mutex_unlock(&state->mutex);

  return result;
}

bool shrmem_update_client(MapState *state, int index, Coordinate coord, int score) {
  pthread_mutex_lock(&state->mutex);

  state->client_heads[index].coord = coord;
  state->client_heads[index].score = score;

  pthread_mutex_unlock(&state->mutex);

  return true;
}

int shrmem_get_game_turns(MapState *state) {
  pthread_mutex_lock(&state->mutex);
  int result = state->game_turns;
  pthread_mutex_unlock(&state->mutex);

  return result;
}

void shrmem_inc_game_turns(MapState *state) {
  pthread_mutex_lock(&state->mutex);
  ++state->game_turns;
  pthread_mutex_unlock(&state->mutex);
}

int shrmem_get_turn_limit(MapState *state, int turn_milisec) {
  pthread_mutex_lock(&state->mutex);
  int result = state->time_limit * 60 * 1000 / turn_milisec;
  pthread_mutex_unlock(&state->mutex);

  return result;
}

