#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define GRID_SIZE 10

void reset_terminal_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, orig_termios);
}

void set_conio_terminal_mode(struct termios *orig_termios) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, orig_termios); 
    new_termios = *orig_termios; 
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable buffering and echoing
    new_termios.c_cc[VMIN] = 1; // Wait for one character
    new_termios.c_cc[VTIME] = 0; // No timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void save_screen() {
    // Save the current screen state using tput
    system("tput smcup");  // Save the screen buffer
}

void restore_screen() {
    // Restore the previously saved screen state using tput
    system("tput rmcup");  // Restore the screen buffer
}

void clear_screen() {
    // Clear the terminal screen using tput
    system("tput clear");
}

void move_cursor(int row, int col) {
    // Move the cursor to a specific position using tput
    printf("\033[%d;%dH", row, col);
    fflush(stdout);
}

void print_grid(char grid[GRID_SIZE][GRID_SIZE], int start_row, int start_col) {
    for (int i = 0; i < GRID_SIZE; i++) {
        move_cursor(start_row + i, start_col);
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
    }
    fflush(stdout);
}

void print_instructions(int start_row, int start_col) {
    const char *instructions[] = {
        "Instructions:",
        "Press space to exit.",
        "Press a number (0-9)",
        "to replace grid symbols."
    };
    int num_instructions = sizeof(instructions) / sizeof(instructions[0]);

    for (int i = 0; i < num_instructions; i++) {
        move_cursor(start_row + i, start_col);
        printf("%s", instructions[i]);
    }
    fflush(stdout);
}

// Function to draw a vertical split
void draw_split(int cols) {
    for (int i = 1; i <= cols; ++i) {
        printf("\033[%d;%dH│", i, GRID_SIZE * 2 + 1); // Draw vertical line
    }
}

/* TODO make moving cursor with tput
void move_cursor(int row, int col) {
    // Move the cursor to a specific position using tput
    char command[100];
    snprintf(command, sizeof(command), "tput cup %d %d", row - 1, col - 1);  // Adjust for 0-based indexing
    system(command);
}
*/

int main() {
    struct termios orig_termios;
    set_conio_terminal_mode(&orig_termios);

    // Save the current terminal screen state
    save_screen();

    // Clear the terminal screen
    clear_screen();
    system("tput civis"); // make cursor invisible

    // Initialize the grid with # symbols
    char grid[GRID_SIZE][GRID_SIZE];
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '#';
        }
    }

    // Grid and instruction positions
    int grid_start_row = 1, grid_start_col = 1;
    int instructions_start_row = 1, instructions_start_col = GRID_SIZE * 2 + 5;

    // Initial display
    print_grid(grid, grid_start_row, grid_start_col);
    draw_split(10);
    print_instructions(instructions_start_row, instructions_start_col);

    char ch;
    while (1) {
        ch = getchar(); // Get a character without buffering

        if (ch == ' ') { // Check if space is pressed
            break;
        }

        if (ch >= '0' && ch <= '9') { // Check if a number key (0-9) is pressed
            // Replace all grid symbols with the pressed number
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    grid[i][j] = ch;
                }
            }

            // Update the grid
            print_grid(grid, grid_start_row, grid_start_col);
        }
    }

    system("tput cnorm"); // make cursor visible

    // Restore the previous terminal screen content
    restore_screen();

    // Restore the original terminal settings
    reset_terminal_mode(&orig_termios);

    return 0;
}
