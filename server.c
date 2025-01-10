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
    snake->body_y[0] = snake->y;
}

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
void generate_fruit(char *board, int *fruit_x, int *fruit_y, int rows, int cols) {
    printf("Generujem ovocie...\n");
    srand(time(NULL));
    int attempts = 0;
    int max_attempts = rows * cols; // Maximálny počet pokusov, kým skončíme

    do {
        *fruit_x = rand() % cols;
        *fruit_y = rand() % rows;
        attempts++;
        printf("Skúšam pozíciu: (%d, %d), pokus: %d\n", *fruit_y, *fruit_x, attempts);

        if (attempts > max_attempts) {
            printf("Nepodarilo sa nájsť voľné miesto pre ovocie po %d pokusoch. Plocha je pravdepodobne plná.\n", attempts);
            return; // Ukončíme generovanie ovocia
        }
    } while (board[*fruit_y * cols + *fruit_x] != '.');

    printf("Ovocie umiestnené na pozícii: (%d, %d)\n", *fruit_y, *fruit_x);
}



void update_board(char *board, int rows, int cols, snake_t *snake, int fruit_x, int fruit_y, char *obstacles) {
    // 1. Skopíruj prekážky do herného sveta
    memcpy(board, obstacles, rows * cols);

    // 2. Aktualizácia hada
    for (int i = 0; i < snake->length; i++) {
        board[snake->body_y[i] * cols + snake->body_x[i]] = (i == 0) ? 'O' : 'o'; // Hlava a telo hada
    }

    // 3. Pridanie ovocia
    board[fruit_y * cols + fruit_x] = 'F';
}


void server_game_loop(int client_socket) {
    int rows, cols, game_mode, time_limit = 0, world_type, obstacle_option = 0;

    // 1. Prijímame typ sveta
    if (recv(client_socket, &world_type, sizeof(world_type), 0) <= 0) {
        perror("Chyba pri prijímaní world_type");
        close(client_socket);
        return;
    }
    printf("typ svete je cislo : %d\n", world_type);

    // 2. Ak sú vybrané prekážky, prijmeme spôsob ich generovania
    if (world_type == 2) {
        if (recv(client_socket, &obstacle_option, sizeof(obstacle_option), 0) <= 0) {
            perror("Chyba pri prijímaní obstacle_option");
            close(client_socket);
            return;
        }
        printf("prijata moznost prekazok: %d\n", obstacle_option);
    }

	
    // 3. Prijímame rozmery hernej plochy
    if (recv(client_socket, &rows, sizeof(rows), 0) <= 0) {
        perror("Chyba pri prijímaní rows");
        close(client_socket);
        return;
    }
    if (recv(client_socket, &cols, sizeof(cols), 0) <= 0) {
        perror("Chyba pri prijímaní cols");
        close(client_socket);
        return;
    }
    printf("Rozmery: rows = %d, cols = %d\n", rows, cols);

    // 4. Dynamická alokácia pre prekážky
    char *obstacles = malloc(rows * cols * sizeof(char));
    if (!obstacles) {
        perror("Chyba pri alokácii pamäte pre obstacles");
        close(client_socket);
        return;
    }
    memset(obstacles, '.', rows * cols);

    // 5. Ak bola vybraná možnosť načítať prekážky zo súboru
    if (world_type == 2 && obstacle_option == 1) {
        char filename[256];
        if (recv(client_socket, filename, sizeof(filename), 0) <= 0) {
            perror("Chyba pri prijímaní filename");
            free(obstacles);
            close(client_socket);
            return;
        }
        printf("Načítavanie prekážok zo súboru: %s\n", filename);
        load_obstacles_from_file(filename, obstacles, rows, cols);
    } else if (world_type == 2 && obstacle_option == 2) {
        generate_obstacles(obstacles, rows, cols);
    }
	
	recv(client_socket, &game_mode, sizeof(game_mode), 0);
	printf("mod hry %d \n", game_mode);
	
	if (game_mode == 2) { 
        recv(client_socket, &time_limit, sizeof(time_limit), 0); 
        printf("Time Mode selected. Time limit: %d seconds.\n", time_limit);
    }
	
    snake_t snake;
    char *board = malloc(rows * cols * sizeof(char));
    int fruit_x, fruit_y;
    int score = 0;
	printf("Teraz by  malo vytvorit hadika a generovat ovocie \n");
    init_snake(&snake, rows, cols);
    generate_fruit(board, &fruit_x, &fruit_y, rows, cols);
	
	time_t start_time = time(NULL);

    while (1) {
        update_board(board, rows, cols, &snake, fruit_x, fruit_y, obstacles);
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
            generate_fruit(board, &fruit_x, &fruit_y, rows, cols);
        }
		if (game_mode == 2) { 
            if (time(NULL) - start_time >= time_limit) {
                printf("Time's up! Game Over.\n");
                break;
            }
        }
		usleep(200000);
    }
	free(obstacles);
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

void load_obstacles_from_file(const char *filename, char *board, int rows, int cols) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Failed to open obstacle file '%s'.\n", filename);
        memset(board, '.', rows * cols); // Ak súbor nie je dostupný, nastavíme prázdnu plochu
        return;
    }

    // Načítavame obsah súboru do poľa board
    int i = 0, j = 0;
    char c;
    while ((c = fgetc(file)) != EOF && i < rows) {
        if (c == '\n') {
            i++;
            j = 0;
        } else if (j < cols) {
            board[i * cols + j] = c;
            j++;
        }
    }

    fclose(file);
    printf("Obstacles loaded successfully from '%s'.\n", filename);
}



void generate_obstacles(char *board, int rows, int cols) {
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        int x = rand() % rows;
        int y = rand() % cols;
        if (board[x * cols + y] == '.') { // Vyhneme sa kolízii s inými prekážkami
            board[x * cols + y] = 'X';
        }
    }
}

