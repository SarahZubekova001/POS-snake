#ifndef CLIENT_H
#define CLIENT_H

void save_game_world(char *filename, char *board, int rows, int cols);
void render_game_world(char *board, int rows, int cols, int score);
char get_player_input();
void display_menu();
int get_menu_choice();
int select_game_mode();
void get_board_size(int *rows, int *cols);

#endif