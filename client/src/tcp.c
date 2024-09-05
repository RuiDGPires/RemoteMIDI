#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include  <signal.h>
#include "tcp.h"

static struct _server_t *server = NULL;
static int connected;

void INThandler(int sig);

struct _server_t {
   int sock;
   struct sockaddr_in addr;
   char buffer[1024];
   cleanup_t cleanup;
};

void tcp_send(message_t message) {
    send(server->sock, &message, sizeof(message), 0);
}

void tcp_connect(char addr[], int port, cleanup_t cleanup) {
    if (server != NULL) {
        perror("Connection has already been established");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, INThandler);
    server = (struct _server_t *) malloc(sizeof(struct _server_t));
    server->cleanup = cleanup;
    
    if (server == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    printf("Creating socket\n");
    // Create socket
    if ((server->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        free(server);
        exit(EXIT_FAILURE);
    }

    // Set server address
    server->addr.sin_family = AF_INET;
    server->addr.sin_port = htons(port);
    printf("Setting address\n");
    if (inet_pton(AF_INET, addr, &(server->addr.sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        free(server);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    printf("Establishing connection\n");
    if (connect(server->sock, (struct sockaddr *)&(server->addr), sizeof(server->addr)) < 0) {
        perror("Connect failed");
        free(server);
        exit(EXIT_FAILURE);
    }

    connected = 1;
    printf("Connected to %s:%d\n", addr, port);
}

void tcp_disconnect() {
    if (!connected) return;
    if (server == NULL) {
        perror("Connection hasn't been established");
        exit(EXIT_FAILURE);
    }

    printf("Disconnecting\n");
    tcp_send(0);
    close(server->sock);
    free(server);
    server = NULL;
    connected = 0;
}

void INThandler(int sig) {
    server->cleanup(); 
    tcp_disconnect();
    signal(sig, SIG_IGN);
    signal(SIGINT, INThandler);
    exit(0);
}
