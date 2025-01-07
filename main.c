#include "server.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int random_in_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void start_game(int rows, int cols, int game_mode) {
    char *board = malloc(rows * cols * sizeof(char));
    snake_t snake;
    int fruit_x, fruit_y;
    int score = 0;
    int time_limit = 0;
    int speed = 100000; // Rýchlosť hry (v mikrosekundách)

    init_snake(&snake, rows, cols);
    srand(time(NULL));
    fruit_x = random_in_range(0, cols - 1);
    fruit_y = random_in_range(0, rows - 1);

    if (game_mode == 2) {
        printf("Enter time limit in seconds: ");
        scanf("%d", &time_limit);
    }

    time_t start_time = time(NULL);

    while (1) {
        system("clear");

        // Aktualizácia herného poľa a zobrazenie
        update_board(board, rows, cols, &snake, fruit_x, fruit_y);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("%c ", board[i * cols + j]);
            }
            printf("\n");
        }
        printf("Score: %d\n", score);

        if (game_mode == 2) {
            int elapsed_time = time(NULL) - start_time;
            printf("Time left: %d seconds\n", time_limit - elapsed_time);
            if (elapsed_time >= time_limit) {
                printf("Time's up! Game Over.\n");
                break;
            }
        }

        // Skontrolujte, či je k dispozícii vstup od hráča na zmenu smeru
        char ch;
        if ((ch = get_player_input()) != -1) {
            if (ch == 'w' && snake.dy == 0) { snake.dx = 0; snake.dy = -1; }
            if (ch == 's' && snake.dy == 0) { snake.dx = 0; snake.dy = 1; }
            if (ch == 'a' && snake.dx == 0) { snake.dx = -1; snake.dy = 0; }
            if (ch == 'd' && snake.dx == 0) { snake.dx = 1; snake.dy = 0; }
            if (ch == 'q') {
                printf("Game over! You quit the game.\n");
                break;
            }
        }

        // Posuňte hadíka
        move_snake(&snake, rows, cols);

        if (check_collision(&snake)) {
            printf("Game Over! You hit yourself.\n");
            break;
        }

        // Ak hadík zje ovocie, zvýši sa skóre a hadík narastie
        if (snake.x == fruit_x && snake.y == fruit_y) {
            grow_snake(&snake, rows, cols);
            score++;
            fruit_x = random_in_range(0, cols - 1);
            fruit_y = random_in_range(0, rows - 1);

        }

        // Časový interval pre pohyb hadíka
        usleep(speed);
    }

    free(board);
    free(snake.body_x);
    free(snake.body_y);
}


int main() {

    while (1) {
        display_menu();
        int choice, rows, cols, game_mode;
        choice = get_menu_choice();

        switch (choice) {
            case 1:
                game_mode = select_game_mode();
                get_board_size(&rows, &cols);
                start_game(rows, cols, game_mode);
                break;

            case 2:
                printf("Feature not implemented yet: Join Existing Game\n");
                break;

            case 3:
                printf("Feature not implemented yet: Resume Game\n");
                break;

            case 4:
                printf("Exiting game. Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}