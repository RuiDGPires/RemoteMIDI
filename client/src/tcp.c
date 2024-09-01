#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcp.h"

struct _server_t {
   int sock;
   struct sockaddr_in addr;
   char buffer[1024];
};

void tcp_send(struct _server_t *server, message_t message) {
    send(server->sock, &message, sizeof(message), 0);
}

struct _server_t *tcp_connect(char addr[], int port) {
    struct _server_t *server = (struct _server_t *) malloc(sizeof(struct _server_t));
    
    if (server == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((server->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        free(server);
        exit(EXIT_FAILURE);
    }

    // Set server address
    server->addr.sin_family = AF_INET;
    server->addr.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &(server->addr.sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        free(server);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(server->sock, (struct sockaddr *)&(server->addr), sizeof(server->addr)) < 0) {
        perror("Connect failed");
        free(server);
        exit(EXIT_FAILURE);
    }

    return server;
}

void tcp_disconnect(struct _server_t **server) {
    close((*server)->sock);
    free(*server);
    *server = NULL;
}

//int main() {
//    server_t *server = tcp_connect(ADDR, PORT);
//    tcp_send(server, "Hello\0");
//    tcp_disconnect(&server);
//    return 0;
//}
