#include "parser.h"

#ifndef SERVER_H
#define SERVER_H

void start_server(StartupArguments* arguments);

void handle_client(int fd);

char* read_header(int fd);

void send_file(int fd, char* file_name);

void interrupt_signal_handler(int a);

void child_signal_handler(int a);

#endif /* SERVER_H */
