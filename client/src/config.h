#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <CCLArgs/cclargs.h>
#include <stdio.h>

typedef struct {
    int verbose;
    char *midi_device_name;
    int midi_device_id;
    int list;
    int allow_all;

    char *server_addr;
    int server_port;

    char *config;
} args_t;

args_t parse_args(ARGS);
args_t parse_config(char *config_path);


#endif
