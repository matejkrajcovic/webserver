#include "parser.h"

#ifndef SERVER_H
#define SERVER_H

void start_server(StartupArguments* arguments);

void handle_client(int fd);

char* read_header(int fd);

void send_file(int fd, char* file_name);

#endif /* SERVER_H */
