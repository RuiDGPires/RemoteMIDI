#include <string.h>
#include <stdio.h>
#include "tcp.h"
#include "midi.h"
#include "config.h"

#define DEFAULT_MIDI_INPUT_ID 1
#define DEFAULT_SERVER_ADDR "192.168.1.8"
#define DEFAULT_SERVER_PORT 8080

#define AMPERO_CONTROL_ID 3
#define AMPERO_CONTROL_NAME "Ampero Control MIDI 1"

void print_info_if_verbose(int verbose, int id, const PmDeviceInfo *info){
    if (!verbose) return;

    printf("id: %d\n", id);
    printf("name: %s\n", info->name);
    printf("input: %s\n", info->input ? "True" : "False");
    printf("output: %s\n", info->output ? "True" : "False");
    printf("virtual: %s\n", info->is_virtual ? "True" : "False");
}

int search(args_t args) {
    if (args.list)
        args.verbose = TRUE;

    if (args.midi_device_name == NULL && args.midi_device_id == -1 && !args.list) {
        args.midi_device_id = DEFAULT_MIDI_INPUT_ID;
        printf("No arguments provided, defaulting to device %d\n", args.midi_device_id);
    }

    int found = 0;
    int id = args.midi_device_id;

    for (int i = 0; i < 20; i++) {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);

        if (info == NULL) 
            continue;

        if (args.midi_device_id != -1 && args.midi_device_id != i)
            continue;
            
        if (args.midi_device_name != NULL)
            if (strcmp(info->name, args.midi_device_name) != 0) 
                continue;

        if (info->input == 0 && !args.allow_all) 
            continue;

        print_info_if_verbose(args.verbose, i, info);

        if (id == -1);
            id = i;
        found++;

    }

    if (!found) {
        printf("No MIDI input device was found with provided arguments\n");
    } else if (found > 1) {
        if (!args.list)
            printf("More than one device was found, defaulting to the first: %d\n", id);
    }

    if (args.list)
        exit(0);

    return id;
}

void handle_message(PmEvent event, void *vctx) {
    tcp_send(event.message);
}

int main(int argc, char *argv[]) {
    args_t args = parse_args(argc, argv);

    int midi_input_device_id = search(args);

    if (args.server_addr == NULL) {
        args.server_addr = DEFAULT_SERVER_ADDR;
        printf("No server address provided, defaulting to %s\n", args.server_addr);
    }

    if (args.server_port == -1) {
        args.server_port = DEFAULT_SERVER_PORT;
        printf("No server port provided, defaulting to %d\n", args.server_port);
    }

    printf("Connecting to %s:%d\n", args.server_addr, args.server_port);
    tcp_connect(args.server_addr, args.server_port, midi_finalize);

    midi_context_t ctx = (midi_context_t){
        .midi_input_device_id = midi_input_device_id,
        .connection_context = NULL,
        .midi_process = handle_message,
    };
    
    midi_process_main(&ctx);

    tcp_disconnect();

    return 0;
}
