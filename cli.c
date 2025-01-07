
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "socket.h"

#define SERVER_NAME "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int clientSocket = connect_to_server(SERVER_NAME, PORT);
    if (clientSocket < 0) {
        fprintf(stderr, "Klient: Nepodarilo sa pripojit k serveru.\n");
        exit(EXIT_FAILURE);
    }
    printf("Klient: Pripojeny k serveru %s:%d.\n", SERVER_NAME, PORT);

    const char *message = "Ahoj, server!";
    if (send(clientSocket, message, strlen(message), 0) < 0) {
        perror("Klient: Chyba pri odosielani spravy");
        active_socket_destroy(clientSocket);
        exit(EXIT_FAILURE);
    }
    printf("Klient: Odoslal som spravu: %s\n", message);

    char buffer[BUFFER_SIZE] = {0};
    ssize_t receivedBytes = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (receivedBytes < 0) {
        perror("Klient: Chyba pri prijimani odpovede");
        active_socket_destroy(clientSocket);
        exit(EXIT_FAILURE);
    }

    printf("Klient: Odpoved od servera: %s\n", buffer);

    active_socket_destroy(clientSocket);
    printf("Klient: Pripojenie ukoncene.\n");
    return 0;
}
