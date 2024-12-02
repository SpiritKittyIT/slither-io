#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "winedit.h"

// Save the current screen state
static void save_screen() {
  system("tput smcup");
}

// Restore the previously saved screen state
static void restore_screen() {
  system("tput rmcup");
}

// Clear the terminal screen
void clear_screen() {
  system("tput clear");
}

// Move the cursor to a specific position
void move_cursor(int row, int col) {
  printf("\033[%d;%dH", row + 1, col + 1); // 1 based indexing
}

// Set the terminal to console input-output (conio) state and save original settings
static void set_conio_terminal_mode(struct termios *orig_termios) {
  struct termios new_termios;
  tcgetattr(STDIN_FILENO, orig_termios); // Save terminal settings
  new_termios = *orig_termios; 
  new_termios.c_lflag &= ~(ICANON | ECHO); // Disable buffering and echoing
  new_termios.c_cc[VMIN] = 1; // Wait for one character
  new_termios.c_cc[VTIME] = 0; // No timeout
  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios); // Set settings
}

static void restore_terminal_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, orig_termios);
}

void hide_cursor() {
    system("tput civis");
}

void show_cursor() {
    system("tput cnorm");
}

void init_game_screen(struct termios *orig_termios) {
  set_conio_terminal_mode(orig_termios);
  save_screen();
  clear_screen();
  hide_cursor();
}

void undo_game_screen(struct termios *orig_termios) {
  restore_terminal_mode(orig_termios);
  restore_screen();
  show_cursor();
}
