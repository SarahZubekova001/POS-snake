#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "socket.h"

#define PORT 12345
#define BUFFER_SIZE 5024

int main() {
    int passiveSocket = passive_socket_init(PORT);
    if (passiveSocket < 0) {
        fprintf(stderr, "Server: Nepodarilo sa inicializovat pasivny socket.\n");
        exit(EXIT_FAILURE);
    }
    printf("Server: Cakam na pripojenie klienta na porte %d...\n", PORT);

    int activeSocket = passive_socket_wait_for_client(passiveSocket);
    if (activeSocket < 0) {
        fprintf(stderr, "Server: Nepodarilo sa akceptovat klienta.\n");
        passive_socket_destroy(passiveSocket);
        exit(EXIT_FAILURE);
    }
    printf("Server: Klient pripojeny!\n");

    char buffer[BUFFER_SIZE] = {0};
    ssize_t receivedBytes = recv(activeSocket, buffer, BUFFER_SIZE - 1, 0);
    if (receivedBytes < 0) {
        perror("Server: Chyba pri prijimani dat");
        active_socket_destroy(activeSocket);
        passive_socket_destroy(passiveSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server: Prijata sprava: %s\n", buffer);

    const char *response = "Server: Sprava prijata!";
    if (send(activeSocket, response, strlen(response), 0) < 0) {
        perror("Server: Chyba pri odosielani odpovede");
    }

    active_socket_destroy(activeSocket);
    passive_socket_destroy(passiveSocket);
    printf("Server: Pripojenie ukoncene.\n");
    return 0;
}
