#pragma once

#include <stdbool.h>

// Function prototype for handling command-line arguments
bool handle_flags(int argc, char *argv[], int *size, bool *with_obstacles, const char **from_file);
