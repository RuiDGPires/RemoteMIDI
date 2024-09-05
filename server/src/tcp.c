#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "tcp.h"

static int server_fd, new_socket, valread;
static struct sockaddr_in address;
static int opt = 1;
static int addrlen = sizeof(address);

void tcp_open_server(int port) {
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    char hostbuffer[256];
    char *ip;
    struct hostent *host_entry;
    int hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);

    // To convert an Internet network
    // address into ASCII string
    ip = inet_ntoa(*((struct in_addr*)
                        host_entry->h_addr_list[0]));

    printf("Server running on %s:%d\n", ip, port);
}

void tcp_receive(void (*callback)(Message)) {
    Message buffer;

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    printf("New client connected\n");
    while (1) {
        // Receive a message from the client
        valread = read(new_socket, &buffer, sizeof(Message));
        if (valread < 0) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }

        if (buffer == 0)
            break;

        callback(buffer);
    }
    close(new_socket);
}

void tcp_close_server() {
    // Close the socket
}
