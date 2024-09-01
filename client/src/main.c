#include <string.h>
#include <stdio.h>
#include <cclargs.h>
#include <pm_common/portmidi.h>
#include "tcp.h"
#include "midi.h"

#define DEFAULT_MIDI_INPUT_ID 1
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

typedef struct {
    int verbose;
    char *midi_device_name;
    int midi_device_id;
    int list;
    int allow_all;
} args_t;

args_t parse_args(ARGS) {
    args_t args = (args_t) {
        .verbose = FALSE,
        .midi_device_name = NULL,
        .midi_device_id = -1,
        .list = FALSE,
        .allow_all = FALSE,
    };

    BEGIN_PARSE_ARGS("")
        ARG_FLAG(args.verbose, "v", "verbose")
        ARG_STRING(args.midi_device_name, "--name")
        ARG_INT(args.midi_device_id, "--id")
        ARG_FLAG(args.list, "l", "list")
        ARG_FLAG(args.allow_all, "a", "all") // Bypass input only filter
    END_PARSE_ARGS

    return args;
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

    tcp_connect("192.168.1.8", 8080, midi_finalize);

    midi_context_t ctx = (midi_context_t){
        .midi_input_device_id = midi_input_device_id,
        .connection_context = NULL,
        .midi_process = handle_message,
    };
    
    midi_process_main(&ctx);

    tcp_disconnect();

    return 0;
}
