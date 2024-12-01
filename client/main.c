#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../utils/map.h"

int main() {
  Map *map = map_init(10, 1);

  map_dispview(map, 10, 5, 5);
  usleep(5000000);
  map->fields[5][5] = FIELD_FOOD;
  map_dispview(map, 10, 5, 5);

  map_destroy(map);
  
  return 0;
}
