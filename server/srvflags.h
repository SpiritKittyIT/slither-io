#pragma once

#include <stdbool.h>

// Function prototype for handling command-line arguments
bool srvflags_handle(int argc, char *argv[], int *size, int *length, bool *with_obstacles, const char **from_file);
