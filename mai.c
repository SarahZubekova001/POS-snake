#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); // Vytvorenie nového procesu

    if (pid < 0) {
        perror("Chyba pri vytvarani procesu");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Toto je detský proces (klient)
        printf("Klient: Spustam klienta...\n");
        execl("./client", "./client", NULL); // Spustenie klienta
        perror("Chyba pri spusteni klienta");
        exit(EXIT_FAILURE);
    } else {
        // Toto je rodičovský proces (server)
        printf("Server: Spustam server...\n");
        execl("./server", "./server", NULL); // Spustenie servera
        perror("Chyba pri spusteni servera");
        exit(EXIT_FAILURE);
    }

    return 0;
}
