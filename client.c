#include "client.h"
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct {
    int socket;
    int rows;
    int cols;
    char *board;
    int running;
	int paused;
} client_data_t;


void display_menu() {
	printf("\033[H\033[J");
    printf("=== Snake Game Menu ===\n");
    printf("1. Start New Game\n");
    printf("2. Resume Game\n");
    printf("3. Exit\n");
}

int get_menu_choice() {
    int choice;
    while (1) {
        printf("Enter your choice: ");
        if (scanf("%d", &choice) == 1 && (choice >= 1 && choice <= 3)) {
            break;
        }
        while (getchar() != '\n'); 
        printf("Invalid input. Please enter a number between 1 and 3.\n");
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

int select_world_type() {
    int world_type;
    printf("\nSelect World Type:\n");
    printf("1. World without obstacles\n");
    printf("2. World with obstacles\n");
    while (1) {
        printf("Enter your choice: ");
        if (scanf("%d", &world_type) == 1 && (world_type == 1 || world_type == 2)) {
            break;
        }
        while (getchar() != '\n');
        printf("Invalid choice. Please enter 1 or 2.\n");
    }
    return world_type;
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

char get_player_input() {
    struct termios oldt, newt;
    char input = -1;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    input = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, 0);

    return input;
}

void render_game_world(const char *board, int rows, int cols, int score) {
    printf("\033[H\033[J");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", board[i * cols + j]);
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
	usleep(16000);
}


void *handle_user_input(void *arg) {
    client_data_t *data = (client_data_t *)arg;
    char input;

    while (data->running) {
        if (!data->paused) { 
            input = get_player_input();
            if (input != -1) {
                if (input == 'q') {
                    data->running = 0;
                    send(data->socket, &input, 1, 0);
                    break;
                } else if (input == 'p') {
                    data->paused = 1;
                    send(data->socket, &input, 1, 0);
                } else {
                    send(data->socket, &input, 1, 0); 
                }
            }
        }
        usleep(100000); 
    }
    return NULL;
}

void *handle_server_updates(void *arg) {
    client_data_t *data = (client_data_t *)arg;

    while (data->running) {
		if (data->paused) { 
            usleep(100000); 
            continue; 
        }
        int received = recv(data->socket, data->board, data->rows * data->cols, 0);
        if (received > 0) {
            render_game_world(data->board, data->rows, data->cols, 0);
        } else {
            data->running = 0;
        }
        int score;
        received = recv(data->socket, &score, sizeof(score), 0);
        if (received <= 0) {
            data->running = 0;
            break;
        }

        render_game_world(data->board, data->rows, data->cols, score);

        usleep(50000);
    }
    return NULL;
}

void start_client() {
	

    // Rodičovský proces pokračuje ako klient
    //printf("Waiting for server to start...\n");
     // Počkať, aby sa server inicializoval
    int client_socket = connect_to_server("localhost", 50200);
    if (client_socket < 0) {
        perror("Failed to connect to server");
        return;
    }

    //printf("Connected to server!\n");
	int world_type = select_world_type();
    send(client_socket, &world_type, sizeof(world_type), 0);
	
	int game_mode = select_game_mode();
    send(client_socket, &game_mode, sizeof(game_mode), 0);
	if (game_mode == 2) { 
        int time_limit;
        printf("Enter time limit in seconds: ");
        while (scanf("%d", &time_limit) != 1 || time_limit <= 0) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a positive number: ");
        }
		send(client_socket, &time_limit, sizeof(time_limit), 0); 
	}
	
    int rows, cols;
    get_board_size(&rows, &cols);
    send(client_socket, &rows, sizeof(rows), 0);
    send(client_socket, &cols, sizeof(cols), 0);

    char *board = malloc(rows * cols * sizeof(char));
    client_data_t data = {client_socket, rows, cols, board, 1, 0};

    pthread_t input_thread, update_thread;

    if (pthread_create(&input_thread, NULL, handle_user_input, &data) != 0) {
        perror("Failed to create input thread");
        free(board);
        close(client_socket);
        return;
    }

    if (pthread_create(&update_thread, NULL, handle_server_updates, &data) != 0) {
        perror("Failed to create update thread");
        data.running = 0;
        pthread_join(input_thread, NULL);
        free(board);
        close(client_socket);
        return;
    }

    while (data.running) {
        if (data.paused) {
            display_menu();
            int choice = get_menu_choice();
            if (choice == 2) { 
                data.paused = 0;
                char resume_signal = 'r';
                send(client_socket, &resume_signal, 1, 0);
            } else if (choice == 3) { 
                data.running = 0;
				char quit_signal = 'q'; 
				send(client_socket, &quit_signal, 1, 0);
				break; 
            }
        }
        usleep(100000);
    }

    pthread_join(input_thread, NULL);
    pthread_join(update_thread, NULL);
	

    printf("Game Over! \n");
	if (game_mode == 2) {
		printf("Time's up! \n");
	}


    free(board);
    close(client_socket);
}


int main() {
	
	display_menu();
    int choice = get_menu_choice();
	
	if(choice == 1){
		pid_t pid = fork();

		if (pid < 0) {
			perror("Failed to fork");
			return 1;
		}

		if (pid == 0) {
			// Detský proces spustí server
			//printf("Starting server process...\n");
			execl("./server", "./server", NULL); 
			perror("Failed to start server");
			exit(1);
		}
		sleep(1);
	}
	start_client();
	return 0;
}