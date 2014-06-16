#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StartupArguments* parse_command_line_arguments(int argc, char** argv) {
    StartupArguments* arguments = malloc(sizeof(StartupArguments));
    if (arguments == NULL) {
        perror("Malloc failed to allocate memory.\n");
        return NULL;
    }

    if (argc != 3) {
        perror("Not enough arguments.\n");
        return NULL;
    }

    arguments->port = atoi(argv[1]);
    arguments->path = argv[2];

    if (!only_numeric(argv[1]) || (arguments->port < 0) || (arguments->port > 65535)) {
        return NULL;
    }

    return arguments;
}

void print_usage(void) {
    printf("USAGE: webserver PORT PATH\n");
    printf("PORT is integer between 0 and 65535\n");
}

int only_numeric(char* str) {
    int len = strlen(str);
    int i;
    for (i = 0; i < len; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}
