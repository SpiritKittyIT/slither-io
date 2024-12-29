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

// Function to create shared memory and initialize Shrmem
Shrmem *shrmem_create(pid_t pid, int map_size, int game_length, bool with_obstacles, const char *map_file) {
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
  size_t struct_size = sizeof(Shrmem) + temp_map->fields_size;

  if (ftruncate(fd, struct_size) == -1) {
    perror("ftruncate");
    close(fd);
    free(temp_map);
    return NULL;
  }

  Shrmem *result = mmap(NULL, struct_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (result == MAP_FAILED) {
    perror("mmap");
    close(fd);
    free(temp_map);
    return NULL;
  }

  close(fd);

  // Initialize synchronization primitives and Shrmem
  pthread_mutexattr_t mutex_attr;
  pthread_condattr_t cond_attr;

  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);

  pthread_condattr_init(&cond_attr);
  pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);

  pthread_mutex_init(&result->mutex, &mutex_attr);
  pthread_cond_init(&result->cond, &cond_attr);

  // Initialize the map in shared memory
  memcpy(&result->map, temp_map, temp_map->fields_size);
  free(temp_map);

  result->game_info.clients = 0;
  result->game_info.game_turns = 0;
  result->game_info.time_limit = game_length;
  result->game_info.game_over = false;

  pthread_mutexattr_destroy(&mutex_attr);
  pthread_condattr_destroy(&cond_attr);

  return result;
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
Shrmem *shrmem_access(pid_t pid, size_t *size) {
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

  *size = shm_stat.st_size;
  Shrmem *result = mmap(NULL, shm_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (result == MAP_FAILED) {
    perror("mmap");
    close(fd);
    return NULL;
  }

  close(fd);
  return result;
}

void shrmem_disconnect(Shrmem *shrmem, size_t size) {
  if (shrmem != NULL) {
    // Unmap the shared memory region
    if (munmap(shrmem, size) == -1) {
      perror("munmap");
    }
  }
}

// Function to notify waiting processes
void shrmem_notify(Shrmem *shrmem) {
  pthread_mutex_lock(&shrmem->mutex);
  pthread_cond_broadcast(&shrmem->cond);
  pthread_mutex_unlock(&shrmem->mutex);
}

Map *shrmem_get_map_init(Shrmem *shrmem) {
  Map *result;
  pthread_mutex_lock(&shrmem->mutex);
  result = calloc(1, sizeof(Map) + shrmem->map.fields_size);
  result->size = shrmem->map.size;
  result->fields_size = shrmem->map.fields_size;
  memcpy(result->fields, shrmem->map.fields, shrmem->map.fields_size);
  pthread_mutex_unlock(&shrmem->mutex);

  return result;
}

GameInfo *shrmem_game_info_init(Shrmem *shrmem) {
  GameInfo *result = calloc(1, sizeof(GameInfo));
  pthread_mutex_lock(&shrmem->mutex);
  *result = shrmem->game_info;
  pthread_mutex_unlock(&shrmem->mutex);

  return result;
}

bool shrmem_get_update(Shrmem *shrmem, Map *map, GameInfo *game_info) {
  pthread_mutex_lock(&shrmem->mutex);
  pthread_cond_wait(&shrmem->cond, &shrmem->mutex);

  // Copy fields from mapstate
  memcpy(map->fields, shrmem->map.fields, shrmem->map.fields_size);

  // Copy game info
  *game_info = shrmem->game_info;

  pthread_mutex_unlock(&shrmem->mutex);

  return true;
}

bool shrmem_get_client_head(Shrmem *shrmem, pid_t pid, ClientHead *client_head) {
  bool found = false;

  pthread_mutex_lock(&shrmem->mutex);
  for (int i = 0; i < shrmem->game_info.clients; i++) {
    if (shrmem->game_info.client_heads[i].client_id == pid) {
      *client_head = shrmem->game_info.client_heads[i];  // Direct structure copy
      found = true;
      break;
    }
  }
  pthread_mutex_unlock(&shrmem->mutex);

  return found;
}

bool shrmem_add_client(Shrmem *shrmem, pid_t pid, Coordinate coord, int score) {
  pthread_mutex_lock(&shrmem->mutex);

  if (shrmem->game_info.clients < MAX_CLIENTS) {
    shrmem->game_info.client_heads[shrmem->game_info.clients].client_id = pid;
    shrmem->game_info.client_heads[shrmem->game_info.clients].coord = coord;
    shrmem->game_info.client_heads[shrmem->game_info.clients].score = score;
    shrmem->game_info.client_heads[shrmem->game_info.clients].alive = true;
    shrmem->game_info.clients++;

    pthread_mutex_unlock(&shrmem->mutex);

    return true;
  }

  pthread_mutex_unlock(&shrmem->mutex);

  return false;
}

Coordinate shrmem_get_spawn(Shrmem *shrmem) {
  pthread_mutex_lock(&shrmem->mutex);
  Coordinate result = get_spawn_location(&shrmem->map);
  pthread_mutex_unlock(&shrmem->mutex);

  return result;
}

bool shrmem_remove_client(Shrmem *shrmem, pid_t pid) {
  bool found = false;
  pthread_mutex_lock(&shrmem->mutex);

  for (int i = 0; i < shrmem->game_info.clients; i++) {
    if (shrmem->game_info.client_heads[i].client_id == pid) {
      --shrmem->game_info.clients;
      shrmem->game_info.client_heads[i] = shrmem->game_info.client_heads[shrmem->game_info.clients];
      found = true;
      break;
    }
  }

  pthread_mutex_unlock(&shrmem->mutex);

  return found;
}

bool shrmem_update_client(Shrmem *shrmem, ClientHead *client_head) {
  pthread_mutex_lock(&shrmem->mutex);

  for (int i = 0; i < shrmem->game_info.clients; i++) {
    if (shrmem->game_info.client_heads[i].client_id == client_head->client_id) {
      shrmem->game_info.client_heads[i] = *client_head;
      break;
    }
  }

  pthread_mutex_unlock(&shrmem->mutex);

  return true;
}

int shrmem_get_game_turns(Shrmem *shrmem) {
  pthread_mutex_lock(&shrmem->mutex);
  int result = shrmem->game_info.game_turns;
  pthread_mutex_unlock(&shrmem->mutex);

  return result;
}

void shrmem_inc_game_turns(Shrmem *shrmem) {
  pthread_mutex_lock(&shrmem->mutex);
  ++shrmem->game_info.game_turns;
  pthread_mutex_unlock(&shrmem->mutex);
}

int shrmem_get_turn_limit(Shrmem *shrmem, int turn_milisec) {
  pthread_mutex_lock(&shrmem->mutex);
  int result = shrmem->game_info.time_limit * 60 * 1000 / turn_milisec;
  pthread_mutex_unlock(&shrmem->mutex);

  return result;
}

void shrmem_set_game_over(Shrmem *shrmem) {
  pthread_mutex_lock(&shrmem->mutex);
  shrmem->game_info.game_over = true;
  pthread_mutex_unlock(&shrmem->mutex);
}

bool shrmem_spawn_food(Shrmem *shrmem, int food_count) {
  pthread_mutex_lock(&shrmem->mutex);
  bool result = spawn_food(&shrmem->map, food_count);
  pthread_mutex_unlock(&shrmem->mutex);

  return result;
}

void shrmem_print(Shrmem *shrmem) {
  pthread_mutex_lock(&shrmem->mutex);
  printf("turns: %d\n", shrmem->game_info.game_turns);
  printf("clients: %d\n", shrmem->game_info.clients);
  for (int i = 0; i < shrmem->game_info.clients; i++) {
    printf("client_id: %d, coord: (%d, %d), score: %d, alive: %d\n",
    shrmem->game_info.client_heads[i].client_id,
    shrmem->game_info.client_heads[i].coord.x,
    shrmem->game_info.client_heads[i].coord.y,
    shrmem->game_info.client_heads[i].score,
    shrmem->game_info.client_heads[i].alive);
  }
  map_print(&shrmem->map);
  pthread_mutex_unlock(&shrmem->mutex);
}
