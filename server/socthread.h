#pragma once

#include "snakelist.h"
#include "../utils/shrmem.h"

typedef struct {
  Shrmem *shrmem;
  SnakeList *snake_list;
  int socfd;
} SocthreadArgs;

void *socthread_start(void *args);