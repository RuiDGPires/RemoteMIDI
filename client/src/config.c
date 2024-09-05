#include <toml-c/header/toml-c.h>
#include "config.h"

args_t parse_args(ARGS) {
    args_t args = (args_t) {
        .verbose = FALSE,
        .midi_device_name = NULL,
        .midi_device_id = -1,
        .list = FALSE,
        .allow_all = FALSE,

        .server_addr = NULL,
        .server_port = -1,

        .config = NULL,
    };

    BEGIN_PARSE_ARGS("")
        ARG_FLAG(args.verbose, "v", "verbose")
        ARG_STRING(args.midi_device_name, "--name")
        ARG_INT(args.midi_device_id, "--id")
        ARG_FLAG(args.list, "l", "list")
        ARG_FLAG(args.allow_all, "a", "all") // Bypass input only filter
                                             
        ARG_STRING(args.server_addr, "--addr") 
        ARG_INT(args.server_port, "--port") 

        ARG_STRING(args.config, "--config") 
    END_PARSE_ARGS

    return args;
}

args_t parse_config(char *config_path) {
    char errbuf[200];

    FILE *file = fopen(config_path, "r");
    if (file == NULL) {
        printf("Error opening file '%s'\n", config_path);
        exit(0);
    }
    toml_table_t *tbl = toml_parse_file(file, errbuf, sizeof(errbuf));

    toml_free(tbl);
    fclose(file);

}
