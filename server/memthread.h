#pragma once

#include <stdbool.h>

#include "../utils/shrmem.h"
#include "srvlogic.h"
#include "snakelist.h"

#define INACTIVE_LIMIT (int)(10 * 1000 / TURN_MILISEC) // 10 seconds

typedef struct {
  Shrmem *shrmem;
  SnakeList *snake_list;
  int socfd;
  int port;
} MemthreadArgs;

void *memthread_start(void *arg);
