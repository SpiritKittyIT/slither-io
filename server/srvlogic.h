#pragma once

#include <stdbool.h>

#include "snakelist.h"
#include "../utils/shrmem.h"

bool srvlogic_snake_kill(Shrmem *shrmem, SnakeList *snake_list, int index);
bool game_turn(Shrmem *shrmem, SnakeList *snake_list);
