#include "parser.h"
#include "server.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char** argv) {
    int err = 0;
    StartupArguments* arguments = parse_command_line_arguments(argc, argv);
    if (arguments == NULL) {
        print_usage();
        return 1;
    } else {
        err = chdir(arguments->path); // changing directory to one selected by user
        if (err == -1) {
            perror("chdir() failed");
            return 1;
        }
        start_server(arguments);
        free(arguments);
        return 0;
    }
}
