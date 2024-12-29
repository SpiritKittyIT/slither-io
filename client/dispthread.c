#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "dispthread.h"
#include "winedit.h"
#include "../utils/map.h"
#include "../utils/snake.h"
#include "../utils/shrmem.h"
#include "../utils/srvlist.h"

#define VIEW_SIZE 10

static char *get_border(int border_size, Coordinate coord) {
  int border_edge = border_size - 1;

  if (coord.x == 0) {
    if (coord.y == 0) {
      return "┌─";
    }
    if (coord.y == border_edge) {
      return "┐";
    }
    return "──";
  }

  if (coord.x == border_edge) {
    if (coord.y == 0) {
      return "└─";
    }
    if (coord.y == border_edge) {
      return "┘";
    }
    return "──";
  }

  if (coord.y == 0) {
    return "│ ";
  }

  if (coord.y == border_edge) {
    return "│";
  }

  return "  ";
}

static void display_border() {
  int border_size = VIEW_SIZE + 2;
  Coordinate coord = {.x = 0, .y = 0};
  char *symbol = NULL;

  for (int x = 0; x < border_size; x++)
  {
    move_cursor(x, 0);
    for (int y = 0; y < border_size; y++)
    {
      coord.x = x;
      coord.y = y;
      symbol = get_border(border_size, coord);
      printf(symbol);
    }
  }
  fflush(stdout);
}

static void display_map(Map *map, Coordinate coord) {
  int row_offset = 1;
  int col_offset = 2;
  Coordinate top_left = {.x = coord.x - VIEW_SIZE / 2, .y = coord.y - VIEW_SIZE / 2};

  for (int y = 0; y < VIEW_SIZE; y++) {
    move_cursor(row_offset + y, col_offset);

    for (int x = 0; x < VIEW_SIZE; x++) {
      Coordinate position = {.x = x + top_left.x, .y = y + top_left.y};
      printf("%s ", field_symbol[map_getfield(map, position)]);
    }

    printf("\n");
  }

  fflush(stdout);
}

static void display_info(GameInfo *game_info, ClientHead *client_head) {
  int col_offset = VIEW_SIZE * 2 + 5;

  int game_duration = game_info->game_turns * TURN_MILISEC / 1000;
  int game_minutes = game_duration / 60;
  int game_seconds = game_duration % 60;

  move_cursor(0, col_offset);
  printf("Time: %d:%02d", game_minutes, game_seconds);

  if (game_info->time_limit > 0) {
    printf(" / %d:%02d", game_info->time_limit, 0);
  }
  move_cursor(1, col_offset);
  printf("PID: %d, Score: %d", client_head->client_id, client_head->score);

  if (!client_head->alive) {
    printf(" (DEAD)");
  }

  if (game_info->game_over) {
    printf(" (GAME OVER)");
  }

  move_cursor(3, col_offset);
  printf("Active players: %d", game_info->clients);

  int row_offset = 4;
  for (int i = 0; i < game_info->clients; i++) {
    move_cursor(row_offset + i, col_offset);
    printf("PID: %d, Score: %d", game_info->client_heads[i].client_id, game_info->client_heads[i].score);
  }

  fflush(stdout);
}

static bool get_client_head(GameInfo *game_info, pid_t pid, ClientHead *client_head) {
  for (int i = 0; i < game_info->clients; i++) {
    if (game_info->client_heads[i].client_id == pid) {
      *client_head = game_info->client_heads[i];
      
      return true;
    }
  }

  return false;
}

void *start_dispthread(void *args) {
  DispthreadArgs *thread_args = args;

  size_t size;
  Shrmem *shrmem = shrmem_access(thread_args->server->pid, &size);
  if (shrmem == NULL) {
    *thread_args->paused = false;
    clear_screen();
    printf("Server inactive\n");
    return NULL;
  }
  Map *map = shrmem_get_map_init(shrmem);
  GameInfo *game_info = shrmem_game_info_init(shrmem);
  ClientHead client_head;
  client_head.coord.x = VIEW_SIZE / 2;
  client_head.coord.y = VIEW_SIZE / 2;
  client_head.alive = true;

  clear_screen();
  display_border();
  display_map(map, client_head.coord);
  display_info(game_info, &client_head);

  bool playing = true;
  while (playing) {
    if (thread_args->server->pid == 0) {
      playing = false;
      continue;
    }

    if (!shrmem_get_update(shrmem, map, game_info)) {
      playing = false;
      continue;
    }

    if (game_info->game_over) {
      playing = false;
    }
    
    if (!get_client_head(game_info, thread_args->client_pid, &client_head)) {
      continue;
    }
    
    if (!client_head.alive) {
      playing = false;
    }

    if (*thread_args->paused) {
      playing = false;
    }

    clear_screen();
    display_border();
    display_map(map, client_head.coord);
    display_info(game_info, &client_head);
  }

  shrmem_disconnect(shrmem, size);
  free(game_info);
  map_destroy(map);

  return NULL;
}
