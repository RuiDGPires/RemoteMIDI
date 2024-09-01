#ifndef __TCP_H__
#define __TCP_H__

#include <stdint.h>

struct _server_t;
typedef struct _server_t *server_t;
typedef int32_t message_t;

server_t tcp_connect(char addr[], int port);
void tcp_send(server_t, message_t);
void tcp_disconnect(server_t*);

#endif
