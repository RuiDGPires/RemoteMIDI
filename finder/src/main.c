#include <portmidi/pm_common/portmidi.h>
#include <stdio.h>
#include <stdlib.h>
#include <CCLArgs/cclargs.h>

int main(ARGS) {
    int verbose = FALSE;
    char *midi_device_name = NULL;
    int midi_device_id = -1;
    int list = FALSE;
    int allow_all = FALSE;
    int input_only = FALSE;
    int output_only  = FALSE;

    BEGIN_PARSE_ARGS("")
        ARG_STRING(midi_device_name, "--name")
        ARG_INT(midi_device_id, "--id")
        ARG_FLAG(list, "l", "list")
        ARG_FLAG(input_only, "i", "input") // Bypass input only filter
        ARG_FLAG(output_only, "o", "output") // Bypass input only filter
    END_PARSE_ARGS

    int found = 0;
    int id = midi_device_id;
    int all = !(input_only || output_only);

    for (int i = 0; i < 120; i++) {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);

        if (info == NULL) 
            continue;

        if (midi_device_id != -1 && midi_device_id != i)
            continue;
            
        if (midi_device_name != NULL)
            if (strcmp(info->name, midi_device_name) != 0) 
                continue;

        if (info->input != 0 && !(all || input_only))
            continue;
        if (info->output != 0 && !(all || output_only))
            continue;

        printf("id: %d\n", i);
        printf("name: %s\n", info->name);
        printf("input: %s\n", info->input ? "True" : "False");
        printf("output: %s\n", info->output ? "True" : "False");
        printf("virtual: %s\n", info->is_virtual ? "True" : "False");

        if (id == -1);
            id = i;
        found++;
    }

    if (!found) {
        printf("No MIDI input device was found with provided arguments\n");
    }
    return 0;
}
