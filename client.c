#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void save_game_world(char *filename, char *board, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Failed to open file \n");
      return;
    } 
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%c ", board[i * cols + j]);
        }
        fprintf(file, "\n");
      } 
    
    fclose(file);
}
void render_game_world(char *board, int rows, int cols, int score) {
    system("clear");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", board[i * cols + j]);
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
}

char get_player_input() {
	struct termios oldt, newt;
    char input = -1;

    // Získanie aktuálneho nastavenia terminálu
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Vypnutie kanonického režimu a echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Nastavenie neblokujúceho čítania
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    // Pokus o čítanie vstupu
    input = getchar();

    // Obnovenie pôvodného nastavenia terminálu
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, 0);

    return input;
}

void display_menu() {
    printf("=== Snake Game Menu ===\n");
    printf("1. Start New Game\n");
    printf("2. Join Existing Game\n");
    printf("3. Resume Game\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
}

int get_menu_choice() {
    int choice;
    printf("Enter your choice: ");
    while (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("Invalid input. Please enter a number: ");
    }
    return choice;
}

int select_game_mode() {
    int mode;
    printf("\nSelect Game Mode:\n");
    printf("1. Standard Mode\n");
    printf("2. Timed Mode\n");
    while (1) {
        printf("Enter your choice: ");
        if (scanf("%d", &mode) == 1 && (mode == 1 || mode == 2)) {
            break;
        }
        while (getchar() != '\n');
        printf("Invalid choice. Please enter 1 for Standard Mode or 2 for Timed Mode.\n");
    }
    return mode;
}

void get_board_size(int *rows, int *cols) {
    while (1) {
        printf("\nEnter the size of the game board:\n");
        printf("Number of rows: ");
        if (scanf("%d", rows) != 1 || *rows <= 0) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a positive number.\n");
            continue;
        }
        printf("Number of columns: ");
        if (scanf("%d", cols) != 1 || *cols <= 0) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a positive number.\n");
            continue;
        }
        break;
    }
}

