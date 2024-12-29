#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "scrjoin.h"
#include "screen.h"
#include "../utils/srvlist.h"
#include "winedit.h"
#include "../utils/srvlist.h"

static void display_scrjoin(int selected, ServerList *srvlist) {
  clear_screen();
  if (selected == 0) {
    printf("\033[7m%s\033[0m\n", "Return to Menu");
  } else {
    printf("%s\n", "Return to Menu");
  }

  if(!srvlist) {
    return;
  }

  for (int i = 0; i < srvlist->active_count; ++i) {
    if (i == selected - 1) {
      printf("\033[7m%d\033[0m\n", srvlist->servers[i].pid);
    } else {
      printf("%d\n", srvlist->servers[i].pid);
    }
  }
}

Screen open_scrjoin(bool *paused, Server *server) {
  ServerList *srvlist = calloc(1, sizeof(ServerList));
  if (!srvlist) {
    return SCR_MENU;
  }

  if (!get_active_server_list(srvlist)) {
    free(srvlist);

    return SCR_MENU;
  }

  Screen screen = SCR_JOIN;
  int selected = 0;
  const int optcount = 1 + srvlist->active_count;

	char c;
  while (screen == SCR_JOIN) {
    display_scrjoin(selected, srvlist);
    
    c = getchar();
    switch (c)
    {
      case 'w': case 'W':
        selected = (selected - 1 + optcount) % optcount; // Wrap around
        break;

      case 's': case 'S':
        selected = (selected + 1) % optcount; // Wrap around
        break;

      case '\n': case ' ':
        // Enter key
        switch (selected)
        {
          case 0:
            screen = SCR_MENU;
            break;
          default:
            *paused = false;
            *server = srvlist->servers[selected - 1];
            screen = SCR_GAME;
            break;
        }
        break;
      
      default:
        break;
    }
  }
  
  free(srvlist);

	return screen;
}
