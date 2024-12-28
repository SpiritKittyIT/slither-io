#pragma once

#include <stdbool.h>

#include "snakelist.h"
#include "../utils/shrmem.h"

bool game_turn(Shrmem *shrmem, SnakeList *snake_list);
