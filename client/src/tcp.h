#ifndef __TCP_H__
#define __TCP_H__

#include <stdint.h>

struct _server_t;
typedef struct _server_t *server_t;
typedef int32_t message_t;
typedef void (*cleanup_t)(void);

void tcp_connect(char addr[], int port, cleanup_t);
void tcp_send(message_t);
void tcp_disconnect();

#endif
