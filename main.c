#include "server.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    display_menu();
    int choice = get_menu_choice();

    switch (choice) {
        case 1: {
            // Klient spúšťa server a následne pokračuje
            printf("Starting New Game...\n");
            start_client("localhost", 50200); // Klient najskôr spustí server a pripojí sa
            break;
        }
        case 2:
            printf("Resume Game feature works only during pause. Start a new game first.\n");
            break;

        case 3:
            printf("Exiting... Goodbye!\n");
            return 0;

        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }

    return 0;
}
