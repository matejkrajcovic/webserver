#include "parser.h"
#include "server.h"
#include <stdlib.h>

int main(int argc, char** argv) {
    StartupArguments* arguments = parse_command_line_arguments(argc, argv);
    if (arguments == NULL) {
        print_usage();
        free(arguments);
        return 1;
    } else {
        start_server(arguments);
        free(arguments);
        return 0;
    }
}
