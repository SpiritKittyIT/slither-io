#pragma once

#include <termios.h>

void clear_screen();
void move_cursor(int row, int col);
void hide_cursor();
void show_cursor();
void init_game_screen(struct termios *orig_termios);
void undo_game_screen(struct termios *orig_termios);
