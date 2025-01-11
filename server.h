#ifndef SERVER_H
#define SERVER_H

#define BUFFER_SIZE 1024

typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    int length;
    int *body_x;
    int *body_y;
} snake_t;


void init_snake(snake_t *snake, int rows, int cols);
void move_snake(snake_t *snake, int rows, int cols);
void grow_snake(snake_t *snake, int rows, int cols);
void generate_fruit(char *board, int *fruit_x, int *fruit_y, int rows, int cols);
void update_board(char *board, int rows, int cols, snake_t *snake, int fruit_x, int fruit_y, char *obstacles);
void generate_obstacles(char *board, int rows, int cols);
int check_collision(snake_t *snake, char *board, int cols);

void send_board_to_client(int socket, char *board, int rows, int cols);
void handle_client_input(int socket, snake_t *snake);
void server_game_loop(int socket);
int start_server(int port);

#endif
