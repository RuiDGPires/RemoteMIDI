#include <stdio.h>
#include <portmidi/pm_common/portmidi.h>
#include <CCLArgs/cclargs.h>
#include "tcp.h"
#include "midi.h"

#define PORT 8080
#define DEVICE 0

int device = -1;
int show_messages = FALSE;

void handle_message(Message message) {
    if (show_messages)
        printf("Client: %x\n", message);

    midi_send(message);
}

int main(ARGS) {
    BEGIN_PARSE_ARGS("")
        ARG_INT(device, "--id")
        ARG_FLAG(show_messages, "s", "show-mgs")
    END_PARSE_ARGS

    printf("%s\n\n----\n", argv[0]);
    
    if (device == -1) {
        printf("No output device provided, defaulting to %d\n", DEVICE);
        device = DEVICE;
    }

    printf("Device: %d\n", device);
    printf("Show messages: %s\n", show_messages? "True" : "False");
    printf("----\n");

    midi_initialize(device);
    tcp_open_server(PORT);

    while (1) {
        tcp_receive(handle_message);
    }
    tcp_close_server();
    midi_finalize();
}
