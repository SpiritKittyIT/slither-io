#pragma once

#include <stdbool.h>

// Function prototype for handling command-line arguments
bool clientflags_handle(int argc, char *argv[], const char **srv_file, const char **map_file);
