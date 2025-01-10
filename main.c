#include "server.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    while (1) {
        display_menu();
        int choice = get_menu_choice();
		
        switch (choice) {
            case 1: {			
                pid_t pid = fork(); 
                
                if (pid < 0) {
                    perror("Failed to fork");
                    exit(1);
                }

                if (pid == 0) {
                    // Toto je child process - spustíme server
                    printf("Starting server process...\n");
                    start_server(12345);
                    exit(0);
                } else {
                    // Toto je parent process - spustíme klienta
                    sleep(1); // Počkáme, kým server začne počúvať
                    printf("Starting client process...\n");
                    start_client("localhost", 12345);
					
					printf("Game ended. Returning to menu...\n");
                }
                break;
            }

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
