#pragma once

#include "snakelist.h"

typedef struct {
  SnakeList *snake_list;
  int socfd;
} SocthreadArgs;

void *socthread_start(void *args);