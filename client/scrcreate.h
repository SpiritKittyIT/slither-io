#pragma once

#include <stdbool.h>
#include <sys/types.h>

#include "screen.h"

Screen open_scrcreate(bool *paused, pid_t *srv_pid, const char *srv_file, const char *map_file);
