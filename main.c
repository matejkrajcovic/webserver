#include "parser.h"
#include "server.h"
#include <stdio.h>

int main(int argc, char** argv) {
    StartupArguments* arguments = parse_command_line_arguments(argc, argv);
    if (arguments == NULL) {
        print_usage();
        return 1;
    } else {
        start_server(arguments);
        return 0;
    }
}
