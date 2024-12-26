#pragma once

#include <stdbool.h>

#include "../utils/shrmem.h"
#include "srvlogic.h"
#include "snakelist.h"

#define TURN_MILISEC 500
#define INACTIVE_LIMIT (int)(10 * 1000 / TURN_MILISEC) // 10 seconds

typedef struct {
  MapState *map_state;
  SnakeList *snake_list;
  int socfd;
} MemthreadArgs;

void *memthread_start(void *arg);
