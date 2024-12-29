#pragma once

#include <stdbool.h>

#include "screen.h"
#include "../utils/srvlist.h"

Screen open_scrmenu(bool *paused, Server *server, const char *srv_file);
