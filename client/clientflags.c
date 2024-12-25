#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include "clientflags.h"

void print_help() {
  printf("Optional arguments:\n");
  printf("  -s, --server <file>     Set the map size (default: 50)\n");
  printf("  -m, --map <file>        Load map from the specified file\n");
  printf("  -h, --help              Print this help message and exit\n");
}

// Function to handle flags
bool clientflags_handle(int argc, char *argv[], const char **srv_file, const char **map_file) {
  // Default values
  *srv_file = NULL;
  *map_file = NULL;

  // Define long options
  static struct option long_options[] = {
    {"srv_file", required_argument, 0, 's'},
    {"map_file", required_argument, 0, 'm'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "s:of:h", long_options, NULL)) != -1) {
    switch (opt) {
      case 's':
        *srv_file = optarg;
        break;
      case 'm':
        *map_file = optarg;
        break;
      case 'h':
        print_help();
        return false;
      default:
        print_help();
        return false;
    }
  }

  return true;
}
