#pragma once

#include <stdbool.h>

#include "screen.h"

Screen open_scrmenu(bool *paused, const char *srv_file);
