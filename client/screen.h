
#pragma once

typedef enum {
  SCR_QUIT,
  SCR_MENU,
  SCR_CREATE,
  SCR_GAME,
  SCR_JOIN,
} Screen;

typedef struct {
  Screen screen;
  int selected;
  bool paused;
} ScreenState;
