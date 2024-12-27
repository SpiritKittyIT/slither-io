#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>

#include "srvlist.h"

static sem_t *initialize_semaphore(const char *sem_name) {
  sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 1);
  if (sem == SEM_FAILED) {
    perror("sem_open");
  }
  return sem;
}

static bool initialize_shared_memory(int *shm_fd, ServerList **server_list, bool *initialized) {
  struct stat shm_stat;

  // Create or open shared memory
  *shm_fd = shm_open(SRVLIST_FILE, O_CREAT | O_RDWR, 0666);
  if (*shm_fd == -1) {
    perror("shm_open");
    return false;
  }

  // Check the shared memory size
  if (fstat(*shm_fd, &shm_stat) == -1) {
    perror("fstat");
    close(*shm_fd);

    return false;
  }

  // Resize if newly created
  *initialized = (shm_stat.st_size == 0);
  if (*initialized && ftruncate(*shm_fd, sizeof(ServerList)) == -1) {
    perror("ftruncate");
    close(*shm_fd);

    return false;
  }

  // Map the shared memory
  *server_list = mmap(NULL, sizeof(ServerList), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_fd, 0);
  if (*server_list == MAP_FAILED) {
    perror("mmap");
    close(*shm_fd);

    return false;
  }

  // Initialize the memory if it is newly created
  if (*initialized) {
    memset(*server_list, 0, sizeof(ServerList));
  }

  return true;
}

static bool add_server_entry(ServerList *server_list, Server server) {
  if (server_list->active_count >= MAX_SERVER_COUNT) {
    fprintf(stderr, "Error: Server list is full.\n");
    return false;
  }

  server_list->servers[server_list->active_count] = server;
  server_list->active_count++;
  printf("Server added: PID = %d, Port = %d\n", server.pid, server.port);
  return true;
}

static bool remove_server_entry(ServerList *server_list, Server server) {
  bool found = false;

  for (int i = 0; i < server_list->active_count; ++i) {
    if (server_list->servers[i].pid == server.pid) {
      server_list->servers[i] = server_list->servers[server_list->active_count - 1];
      memset(&server_list->servers[server_list->active_count - 1], 0, sizeof(Server));
      --server_list->active_count;
      found = true;

      break;
    }
  }

  if (found) {
    printf("Server removed: PID = %d, Port = %d\n", server.pid, server.port);
  }
  else {
    printf("Server not found: PID = %d, Port = %d\n", server.pid, server.port);
  }
  return found;
}

bool add_server_to_shared_memory(Server new_server) {
  int shm_fd;
  ServerList *server_list;
  sem_t *sem;
  bool initialized;

  // Initialize semaphore
  sem = initialize_semaphore(SEM_FILE);
  if (!sem) {
    return false;
  }

  // Acquire semaphore
  if (sem_wait(sem) == -1) {
    perror("sem_wait");
    sem_close(sem);
    
    return false;
  }

  // Initialize shared memory
  if (!initialize_shared_memory(&shm_fd, &server_list, &initialized)) {
    sem_post(sem);  // Release semaphore before returning
    sem_close(sem);

    return false;
  }

  // Add the server entry
  bool result = add_server_entry(server_list, new_server);

  // Release semaphore
  if (sem_post(sem) == -1) {
    perror("sem_post");
    result = false;
  }

  // Cleanup
  munmap(server_list, sizeof(ServerList));
  close(shm_fd);
  sem_close(sem);

  return result;
}

bool remove_server_from_shared_memory(Server new_server) {
  int shm_fd;
  ServerList *server_list;
  sem_t *sem;
  bool initialized;

  // Initialize semaphore
  sem = initialize_semaphore(SEM_FILE);
  if (!sem) {
    return false;
  }

  // Acquire semaphore
  if (sem_wait(sem) == -1) {
    perror("sem_wait");
    sem_close(sem);

    return false;
  }

  // Initialize shared memory
  if (!initialize_shared_memory(&shm_fd, &server_list, &initialized)) {
    sem_post(sem);  // Release semaphore before returning
    sem_close(sem);

    return false;
  }

  // Remove the server entry
  bool result = remove_server_entry(server_list, new_server);

  // If no servers left, clean up the shared memory and semaphore
  if (server_list->active_count == 0) {
    if (shm_unlink(SRVLIST_FILE) == -1) {
      perror("shm_unlink");
      result = false;
    }

    if (sem_unlink(SEM_FILE) == -1) {
      perror("sem_unlink");
      result = false;
    }
  }

  // Release semaphore
  if (sem_post(sem) == -1) {
    perror("sem_post");
    result = false;
  }

  // Cleanup
  munmap(server_list, sizeof(ServerList));
  close(shm_fd);
  sem_close(sem);

  return result;
}

static int server_pid_compare(const void *a, const void *b) {
  const Server *server_a = a;
  const Server *server_b = b;
  return server_a->pid - server_b->pid;
}

bool get_active_server_list(ServerList *srvlist) {
  int shm_fd;
  sem_t *sem;
  ServerList *server_list;
  ServerList *srvlist_tmp = calloc(1, sizeof(ServerList));

  if (!srvlist_tmp) {
    perror("Failed to allocate memory for ServerList");

    return false;
  }
  srvlist_tmp->active_count = 0;
  memcpy(srvlist, srvlist_tmp, sizeof(ServerList));
  free(srvlist_tmp);

  // Try to open the semaphore
  sem = sem_open(SEM_FILE, O_RDWR);
  if (sem == SEM_FAILED) {
    if (errno == ENOENT) {
      // Semaphore file does not exist
      return true;
    }
    perror("sem_open");
    return false;
  }

  // Acquire the semaphore to prevent race condition
  if (sem_wait(sem) == -1) {
    perror("sem_wait");
    sem_close(sem);

    return false;
  }

  // Open the shared memory object
  shm_fd = shm_open(SRVLIST_FILE, O_RDONLY, 0666);
  if (shm_fd == -1) {
    if (errno == ENOENT) {
      // Shared memory file does not exist
      sem_post(sem); // Release semaphore
      sem_close(sem);
      return true;
    }
    perror("shm_open");
    sem_post(sem); // Release semaphore
    sem_close(sem);
    return false;
  }

  // Map the shared memory object into the address space
  server_list = mmap(NULL, sizeof(ServerList), PROT_READ, MAP_SHARED, shm_fd, 0);
  if (server_list == MAP_FAILED) {
    perror("mmap");
    sem_post(sem); // Release semaphore
    sem_close(sem);
    close(shm_fd);

    return false;
  }

  // Copy the contents of the shared memory
  memcpy(srvlist, server_list, sizeof(ServerList));

  // Sort the servers by PID
  qsort(srvlist->servers, srvlist->active_count, sizeof(Server), server_pid_compare);

  // Cleanup
  munmap(server_list, sizeof(ServerList));
  close(shm_fd);

  // Release the semaphore after accessing shared memory
  sem_post(sem);
  sem_close(sem);

  return true;
}

bool read_and_print_shared_memory() {
  ServerList *srvlist = calloc(1, sizeof(ServerList));
  if (!srvlist) {
    return false;
  }
  if (!get_active_server_list(srvlist)) {
    free(srvlist);

    return false;
  }

  // Print the contents of the shared memory
  printf("Active servers: %d\n", srvlist->active_count);
  for (int i = 0; i < srvlist->active_count; i++) {
    printf("Server %d: PID = %d, socfd = %d, Port = %d\n", i + 1,
           srvlist->servers[i].pid, srvlist->servers[i].socfd, srvlist->servers[i].port);
  }

  // Cleanup
  free(srvlist);

  return true;
}
