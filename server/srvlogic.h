#pragma once

#include <stdbool.h>

#include "snakelist.h"
#include "../utils/shrmem.h"

bool game_turn(MapState *map_state, SnakeList *snake_list);
