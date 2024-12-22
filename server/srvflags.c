#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include "srvflags.h"

void print_help() {
  printf("Optional arguments:\n");
  printf("  -s, --size <int>        Set the map size (default: 50)\n");
  printf("  -o, --obstacles         Add random obstacles to the map\n");
  printf("  -f, --from_file <file>  Load map from the specified file\n");
  printf("  -h, --help              Print this help message and exit\n");
}

// Function to handle flags
bool handle_flags(int argc, char *argv[], int *size, bool *with_obstacles, const char **from_file) {
  // Default values
  *size = 10;
  *with_obstacles = false;
  *from_file = NULL;

  // Define long options
  static struct option long_options[] = {
    {"size", required_argument, 0, 's'},
    {"obstacles", no_argument, 0, 'o'},
    {"from_file", required_argument, 0, 'f'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "s:of:h", long_options, NULL)) != -1) {
    switch (opt) {
      case 's':
        *size = atoi(optarg);
        if (*size <= 0) {
          fprintf(stderr, "Error: Size must be a positive integer.\n");
          return false;
        }
        break;
      case 'o':
        *with_obstacles = true;
        break;
      case 'f':
        *from_file = optarg;
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
