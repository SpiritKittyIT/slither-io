#pragma once

#include <sys/types.h>

typedef struct {
  pid_t srv_pid;
  pid_t client_pid;
} DispthreadArgs;

void *start_dispthread(void *args);
