#ifndef PARSER_H
#define PARSER_H

typedef struct StartupArguments {
    int port;
    char* path;
} StartupArguments;

StartupArguments* parse_command_line_arguments(int argc, char** argv);

void print_usage(void);

int only_numeric(char* str);

#endif /* PARSER_H */
