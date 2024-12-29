#pragma once

#include <stdbool.h>
#include <sys/types.h>

#include "screen.h"
#include "../utils/srvlist.h"

Screen open_scrjoin(bool *paused, Server *server);
