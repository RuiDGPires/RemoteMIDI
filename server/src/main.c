#include <stdio.h>
#include <portmidi/pm_common/portmidi.h>
#include <CCLArgs/cclargs.h>
#include "tcp.h"
#include "midi.h"

#define PORT 8080
#define DEVICE 0

void handle_message(Message message) {
    printf("Client: %x\n", message);
    midi_send(message);
}

int main(ARGS) {
    int device = -1;

    BEGIN_PARSE_ARGS("")
        ARG_INT(device, "--id")
    END_PARSE_ARGS
    
    if (device == -1) {
        printf("No output device provided, defaulting to %d\n", DEVICE);
        device = DEVICE;
    }

    midi_initialize(device);
    tcp_open_server(PORT);
    while (1) {
        tcp_receive(handle_message);
    }
    tcp_close_server();
    midi_finalize();
}
