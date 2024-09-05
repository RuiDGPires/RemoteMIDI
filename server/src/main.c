#include <stdio.h>
#include <portmidi/pm_common/portmidi.h>
#include "tcp.h"
#include "midi.h"

#define PORT 8080

void handle_message(Message message) {
    printf("Client: %x\n", message);
    midi_send(message);
}

int main() {
    int id = Pm_GetDefaultOutputDeviceID();
    midi_initialize(id);
    tcp_open_server(PORT);
    while (1) {
        tcp_receive(handle_message);
    }
    tcp_close_server();
    midi_finalize();
}
