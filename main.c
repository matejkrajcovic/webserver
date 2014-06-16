#include "parser.h"
#include <stdio.h>

int main(int argc, char** argv) {
    StartupArguments* arguments = parse_command_line_arguments(argc, argv);
    if (arguments == NULL) {
        print_usage();
        return 1;
    } else {
        printf("port: %d\n", arguments->port);
        printf("path to root directory: %s\n", arguments->path);
        return 0;
    }
}
