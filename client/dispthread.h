#pragma once

#include <sys/types.h>
#include <stdbool.h>

#include "../utils/srvlist.h"

typedef struct {
  Server *server;
  pid_t client_pid;
  bool *paused;
} DispthreadArgs;

void *start_dispthread(void *args);
