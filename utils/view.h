#pragma once

#include "map.h"

typedef struct {
  int size;
  Field **fields;
  Coordinate top_left;
} View;

View *view_init(int size, Coordinate top_left);
void view_destroy(View *view);
