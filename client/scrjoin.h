#pragma once

#include <stdbool.h>
#include <sys/types.h>

#include "screen.h"

Screen open_scrjoin(bool *paused, pid_t *srv_pid);
