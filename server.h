#include "parser.h"

#ifndef SERVER_H
#define SERVER_H

void start_server(StartupArguments* arguments);

void handle_client(int fd);

char* read_header(int fd);

#endif /* SERVER_H */
