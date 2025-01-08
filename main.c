#include "client.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void start_new_game() {
    pid_t pid = fork(); // Vytvorenie nového procesu

    if (pid < 0) {
        perror("Failed to fork");
        exit(1);
    }

    if (pid == 0) {
        // Toto je child process - spustíme server
        printf("Starting server process...\n");
        start_server(12345);
        exit(0); // Po skončení servera ukončíme proces
    } else {
        // Toto je parent process - spustíme klienta
        sleep(1); // Počkáme, kým server začne počúvať
        printf("Starting client process...\n");
        start_client("localhost", 12345);

        // Po skončení klienta skontrolujeme, či server proces stále beží
        int status;
        waitpid(pid, &status, 0); // Počkáme na ukončenie serverového procesu
        printf("Game ended. Returning to menu...\n");
    }
}

int main() {
    while (1) {
        display_menu();
        int choice = get_menu_choice();

        switch (choice) {
            case 1:
                start_new_game();
                break;

            case 2:
                printf("Resume Game feature is not implemented yet.\n");
                break;

            case 3:
                printf("Exiting... Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
