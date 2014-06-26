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

char* parse_get_header(char* header) {
    char* position = strstr(header, "GET"); // searches for substring
    char* path;
    char* path_start_position = NULL;
    int path_len;

    if (position == NULL) { // not found
        return NULL;
    } else {
        while ((*position != '\0') && (*position != '/')) {
            position++; // skips to start of path
        }
        path_start_position = position;
        path_len = 0;
        while ((position[path_len] != '\0') && (position[path_len] != ' ')) {
            path_len++; // counts length of path
        }
        path = malloc(sizeof(char) * (path_len + 1));
        strncpy(path, path_start_position, path_len);
        path[path_len+1] = '\0';
        return path;
    }
}
