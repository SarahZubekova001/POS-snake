#include "server.h"
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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
        snake->x = cols - 1; 
    } else if (snake->x >= cols) {
        snake->x = 0; 
    }

    snake->y += snake->dy;
    if (snake->y < 0) {
        snake->y = rows - 1; 
    } else if (snake->y >= rows) {
        snake->y = 0; 
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



void server_game_loop(int client_socket) {
    int rows, cols, game_mode, time_limit = 0;

    // Prijímame rozmery hernej plochy od klienta
    recv(client_socket, &rows, sizeof(rows), 0);
    recv(client_socket, &cols, sizeof(cols), 0);
	recv(client_socket, &game_mode, sizeof(game_mode), 0);
	if (game_mode == 2) { 
        recv(client_socket, &time_limit, sizeof(time_limit), 0); 
        printf("Time Mode selected. Time limit: %d seconds.\n", time_limit);
    }
	
    snake_t snake;
    char *board = malloc(rows * cols * sizeof(char));
    int fruit_x, fruit_y;
    int score = 0;

    init_snake(&snake, rows, cols);
    generate_fruit(&fruit_x, &fruit_y, rows, cols);
	
	time_t start_time = time(NULL);

    while (1) {
        update_board(board, rows, cols, &snake, fruit_x, fruit_y);
        send(client_socket, board, rows * cols, 0);
		send(client_socket, &score, sizeof(score), 0);
        char input;
        if (recv(client_socket, &input, 1, MSG_DONTWAIT) > 0) {
            if (input == 'w' && snake.dy == 0) { snake.dx = 0; snake.dy = -1; }
            if (input == 's' && snake.dy == 0) { snake.dx = 0; snake.dy = 1; }
            if (input == 'a' && snake.dx == 0) { snake.dx = -1; snake.dy = 0; }
            if (input == 'd' && snake.dx == 0) { snake.dx = 1; snake.dy = 0; }
            if (input == 'q') {
                printf("Client disconnected.\n");
                break;
            }
        }

        move_snake(&snake, rows, cols);
        if (check_collision(&snake)) {
            printf("Game Over! Collision detected.\n");
            break;
        }

        if (snake.x == fruit_x && snake.y == fruit_y) {
            grow_snake(&snake, rows, cols);
            score++;
            generate_fruit(&fruit_x, &fruit_y, rows, cols);
        }
		if (game_mode == 2) { 
            if (time(NULL) - start_time >= time_limit) {
                printf("Time's up! Game Over.\n");
                break;
            }
        }
		usleep(200000);
    }

    free(board);
    free(snake.body_x);
    free(snake.body_y);
}


int start_server(int port) {
    int server_socket = passive_socket_init(port);
    if (server_socket < 0) {
        perror("Failed to start server.\n");
        return -1;
    }

    printf("Waiting for a client to connect...\n");
    int client_socket = passive_socket_wait_for_client(server_socket);
    if (client_socket < 0) {
        perror("Failed to accept client.\n");
        return -1;
    }

    printf("Client connected. Starting game...\n");
    server_game_loop(client_socket);

    close(client_socket);
    close(server_socket);
    return 0;
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


//void update_player_position(int *x, int *y, int dx, int dy, int rows, int cols) {
  //  *x += dx;
    //*y += dy;

    //if (*x < 0) {
      //  *x = cols - 1;
    //} else if (*x >= cols) {
      //  *x = 0;
    //}
    //if (*y < 0) {
      //  *y = rows - 1;
    //} else if (*y >= rows) {
      //  *y = 0; 
    //}
//}
