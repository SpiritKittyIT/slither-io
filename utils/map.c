#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "map.h"

const char *field_symbol[] = {
	[FIELD_WALL] = "⌗", //⌗⏹
	[FIELD_NONE] = " ",
	[FIELD_FOOD] = "•", //•⏺
	[FIELD_HEAD] = "፠",
	[FIELD_BODY] = "፨",
};

static bool calculate_map_size(const char *path, int *size) {
  if (!path) {
    return false;
  }

  FILE *file = fopen(path, "r");
  if (!file) {
    perror("Failed to open file");
    return false;
  }

  int rows = 0, cols = 0, current_cols = 0;
  char c;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      if (cols == 0) {
        cols = current_cols; // First row sets the column count
      } else if (current_cols != cols) {
        fprintf(stderr, "Inconsistent row lengths in file\n");
        fclose(file);
        return false;
      }
      current_cols = 0;
      rows++;
    } else {
      current_cols++;
    }
  }

  // Check the last line if it doesn't end with a newline
  if (current_cols > 0) {
    if (cols == 0) {
      cols = current_cols;
    } else if (current_cols != cols) {
      fprintf(stderr, "Inconsistent row lengths in file\n");
      fclose(file);
      return false;
    }
    rows++;
  }

  fclose(file);

  if (rows != cols) {
    fprintf(stderr, "File does not represent a square map\n");
    return false;
  }

  *size = rows;
  return true;
}

bool load_fields_from_file(Map *map, const char *path) {
  FILE *file = fopen(path, "r");
  if (!file) {
    perror("Failed to open file");
    return false;
  }

  int total_fields = map->size * map->size;
  int index = 0;
  char c;
  Field field;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      continue;
    }
    field = (Field)c - '0';

    if (field < FIELD_WALL || field > FIELD_BODY) {
      fprintf(stderr, "Invalid field value in file: %c\n", c);
      fclose(file);
      return false;
    }

    if (index >= total_fields) {
      fprintf(stderr, "File contains more data than expected\n");
      fclose(file);
      return false;
    }

    map->fields[index++] = field;
  }

  if (index != total_fields) {
    fprintf(stderr, "File contents do not match data for map size\n");
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}

Map *map_new(int size, bool with_obstacles, const char *from_file) {
  srand(time(NULL));
  // If loading from a file, determine size dynamically
  if (from_file) {
    if (!calculate_map_size(from_file, &size)) {
      return NULL;
    }
  }

  size_t fields_size = sizeof(Field*) * size * size;
  Map *result = calloc(1, sizeof(Map) + fields_size);

  if (!result) {
    perror("Failed to allocate memory for Map");
    return NULL;
  }

  result->size = size;
  result->fields_size = fields_size;

  if (from_file) {
    if (!load_fields_from_file(result, from_file)) {
      free(result);
      return NULL;
    }
  } else {
    int total_fields = result->size * result->size;
    for (int i = 0; i < total_fields; i++) {
      Field field = with_obstacles && rand() % 100 < OBSTACLE_PERCENT ? FIELD_WALL : FIELD_NONE;
      result->fields[i] = field;
    }
  }

  return result;
}

void map_destroy(Map *map) {
  free(map);
}

Field map_getfield(Map *map, Coordinate coord) {
  if (coord.x < 0 || coord.x >= map->size || coord.y < 0 || coord.y >= map->size) {
    return FIELD_WALL;
  }
  
  return map->fields[coord.y * map->size + coord.x];
}

bool map_setfield(Map *map, Coordinate coord, Field field) {
  if (coord.x < 0 || coord.x >= map->size || coord.y < 0 || coord.y >= map->size) {
    return false;
  }

  map->fields[coord.y * map->size + coord.x] = field;
  
  return true;
}

bool spawn_food(Map *map, int food_count) {
  int total_fields = map->size * map->size;

  int empty_count = 0;
  int existing_food_count = 0;
  for (int i = 0; i < total_fields; i++) {
    if (map->fields[i] == FIELD_NONE) {
      ++empty_count;
    }
    if (map->fields[i] == FIELD_FOOD) {
      ++existing_food_count;
    }
  }

  for (int i = 0; i < food_count - existing_food_count; i++) {
    // If no empty fields are available, return false
    if (empty_count == 0) {
      fprintf(stderr, "No empty field available to place food\n");
      return false;
    }

    // Select a random FIELD_NONE
    int target_index = rand() % empty_count;
    for (int i = 0; i < total_fields; i++) {
      if (map->fields[i] == FIELD_NONE) {
        if (target_index == 0) {
          map->fields[i] = FIELD_FOOD;
          return true;
        }
        --target_index;
      }
    }

    --empty_count;
  }

  return false;
}

Coordinate get_spawn_location(Map *map) {
  Coordinate spawn_location = { __INT_MAX__, __INT_MAX__ };
  int total_fields = map->size * map->size;

  // Count the number of FIELD_NONE fields
  int empty_count = 0;
  for (int i = 0; i < total_fields; i++) {
    if (map->fields[i] == FIELD_NONE) {
      empty_count++;
    }
  }

  // If no empty fields are available
  if (empty_count == 0) {
    fprintf(stderr, "No empty field available to place food\n");
    return spawn_location;
  }

  // Select a random FIELD_NONE
  int target_index = rand() % empty_count;
  for (int i = 0; i < total_fields; i++) {
    if (map->fields[i] == FIELD_NONE) {
      if (target_index == 0) {
        spawn_location.x = i % map->size;
        spawn_location.y = i / map->size;
        return spawn_location;
      }
      target_index--;
    }
  }

  return spawn_location;
}

void map_print(Map *map) {
  printf("size: %d\n", map->size);
  for (int y = 0; y < map->size; y++) {
    for (int x = 0; x < map->size; x++) {
        printf("%s ", field_symbol[map->fields[y * map->size + x]]);
    }
    printf("\n");
  }
}
