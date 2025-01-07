#ifndef SERVER_H
#define SERVER_H

typedef struct {
    int x, y;   
    int dx, dy; 
    int length; 
    int *body_x;
    int *body_y;
} snake_t;

void init_snake(snake_t *snake, int rows, int cols);
void move_snake(snake_t *snake, int rows, int cols);
void grow_snake(snake_t *snake, int rows, int cols);
void generate_fruit(int *fruit_x, int *fruit_y, int rows, int cols);
void update_board(char *board, int rows, int cols, snake_t *snake, int fruit_x, int fruit_y);

void load_game_world(char *file, char *board, int rows, int cols);
int check_collision(snake_t *snake);

#endif