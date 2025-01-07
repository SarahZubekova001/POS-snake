#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "socket.h"

int passive_socket_init(const int port) {
  int passSock;
  struct sockaddr_in servAddr;
  // Vytvorenie schránky pre komunikáciu cez internet
  passSock = socket(AF_INET, SOCK_STREAM, 0);
  if (passSock < 0) {
    perror("Chyba pri vytvarani schranky");
    return -1;
  }
  // Nastavenie adresy pre potreby komunikácie
  memset((char*)&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(port);
  if (bind(passSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
    perror("Chyba pri nastavovani adresy schranky");
    return -1;
  }
  // Vytvorenie pasívnej schránky pre prijímanie pripojení
  listen(passSock, 5);
  return passSock;
}

int passive_socket_wait_for_client(int passiveSocket) {
  struct sockaddr_in clAddr;
  socklen_t clSize = sizeof(clAddr);
  int actSock = accept(passiveSocket, (struct sockaddr*)&clAddr, &clSize);
  if (actSock < 0) {
    perror("Chyba pri akceptacii schranky!");
    return -1;
  }
  return actSock;
}

void passive_socket_destroy(int socket) {
  close(socket);
}
 
int connect_to_server(const char *name, const int port) {
    struct addrinfo *server;
    struct addrinfo hints;

    // Inicializácia štruktúry hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Prevod portu na text
    char portText[10] = {0};
    sprintf(portText, "%d", port);

    // Zisťovanie adresy servera podľa mena
    int s = getaddrinfo(name, portText, &hints, &server);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    // Prechádzanie zoznamu dostupných adries
    for (struct addrinfo *rp = server; rp != NULL; rp = rp->ai_next) {
        // Vytvorenie socketu
        int actSock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (actSock < 0) {
            continue;
        }

        // Pokus o pripojenie
        if (connect(actSock, rp->ai_addr, rp->ai_addrlen) == 0) {
            freeaddrinfo(server);
            return actSock; // Úspešné pripojenie
        }

        close(actSock);
    }

    perror("Chyba pripojenia na server!");
    freeaddrinfo(server);
    return -1;
}


void active_socket_destroy(int socket) {
  close(socket);
}
