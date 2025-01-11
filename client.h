#ifndef CLIENT_H
#define CLIENT_H
#define BUFFER_SIZE 1024


void render_game_world(const char *board, int rows, int cols, int score);
char get_player_input();
void display_menu();
int get_menu_choice();
int select_game_mode();
void get_board_size(int *rows, int *cols);
int select_world_type();
void *handle_user_input(void *arg);
void *handle_server_updates(void *arg);
void start_client(const char *server_address, int port);
#endif