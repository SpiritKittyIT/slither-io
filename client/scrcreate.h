#pragma once

#include <stdbool.h>
#include <sys/types.h>

#include "screen.h"
#include "../utils/srvlist.h"

Screen open_scrcreate(bool *paused, Server *server, const char *srv_file, const char *map_file);
