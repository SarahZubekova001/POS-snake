#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void init_snake(snake_t *snake, int rows, int cols) {
    snake->x = cols / 2;
    snake->y = rows / 2;
    snake->dx = 0;
    snake->dy = -1;
    snake->length = 1; 
    snake->body_x = malloc(rows * cols * sizeof(int));
    snake->body_y = malloc(rows * cols * sizeof(int));
    snake->body_x[0] = snake->x;
    snake->body_y[0] = snake->y;
}
void move_snake(snake_t *snake, int rows, int cols) {

    snake->x += snake->dx;
    if (snake->x < 0) {
        snake->x = cols - 1; // Posun na pravý okraj
    } else if (snake->x >= cols) {
        snake->x = 0; // Posun na ľavý okraj
    }

    snake->y += snake->dy;
    if (snake->y < 0) {
        snake->y = rows - 1; // Posun na spodný okraj
    } else if (snake->y >= rows) {
        snake->y = 0; // Posun na horný okraj
    }

  
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body_x[i] = snake->body_x[i - 1];
        snake->body_y[i] = snake->body_y[i - 1];
    }

    snake->body_x[0] = snake->x;
    snake->body_y[0] = snake->y;}

int check_collision(snake_t *snake) {
    for (int i = 1; i < snake->length; i++) {
        if (snake->x == snake->body_x[i] && snake->y == snake->body_y[i]) {
            return 1;
        }
    }
    return 0; 
}
void grow_snake(snake_t *snake, int rows, int cols) {
    if (snake->length < rows * cols) {
        snake->length++;
    }
}
void generate_fruit(int *fruit_x, int *fruit_y, int rows, int cols) {
    srand(time(NULL));
    *fruit_x = rand() % cols;
    *fruit_y = rand() % rows;
}

void update_board(char *board, int rows, int cols, snake_t *snake, int fruit_x, int fruit_y) {
    memset(board, '.', rows * cols);

    for (int i = 0; i < snake->length; i++) {
        board[snake->body_y[i] * cols + snake->body_x[i]] = (i == 0) ? 'O' : 'o'; 
    }
    board[fruit_y * cols + fruit_x] = 'F';
}

void load_game_world(char *filename, char *board, int rows, int cols) {
  FILE *file = fopen(filename, "r");
  if(!file){
    printf("Failed to open file \n");
    return;
  } 
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, " %c", &board[i * cols + j]);
        }
    }
  fclose(file);
}


void update_player_position(int *x, int *y, int dx, int dy, int rows, int cols) {
    *x += dx;
    *y += dy;

    if (*x < 0) {
        *x = cols - 1;
    } else if (*x >= cols) {
        *x = 0;
    }
    if (*y < 0) {
        *y = rows - 1;
    } else if (*y >= rows) {
        *y = 0; 
    }
}