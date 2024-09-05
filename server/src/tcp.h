#ifndef __TCP_H__
#define __TCP_H__

#include <stdint.h>

typedef int32_t Message;

void tcp_open_server(int port);
void tcp_receive(void (*callback)(Message));
void tcp_close_server();

#endif
