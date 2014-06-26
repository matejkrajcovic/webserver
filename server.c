#include "server.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BACK_LOG 10 // number of waiting clients
#define MAX_HEADER_SIZE 2048 // maximum size of html header client sends

void start_server(StartupArguments* arguments) {
    int err = 0;
    int fd, newfd;
    struct sockaddr_in me, peer;
    socklen_t peerlen;

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket() failed");
        exit(1);
    }

    me.sin_family = AF_INET;
    me.sin_addr.s_addr = INADDR_ANY;
    me.sin_port = htons(arguments->port);

    err = bind(fd, (struct sockaddr*) &me, sizeof(me));
    if (err == -1) {
        perror("bind() failed");
        exit(1);
    }

    err = listen(fd, BACK_LOG);
    if (err == -1) {
        perror("listen() failed");
        exit(1);
    }

    printf("Waiting for connection...\n");

    while (1) {
        peerlen = sizeof(peer);
        newfd = accept(fd, (struct sockaddr*) &peer, &peerlen);
        if (newfd == -1) {
            perror("accept() failed");
        }
        printf("Connection accepted.\n");

        handle_client(newfd);

        err = close(newfd);
        if (err == -1) {
            perror("close() failed");
        }

        printf("Connection closed.\n");
    }

    err = close(fd);
    if (err == -1) {
        perror("close() failed");
    }

}

void handle_client(int fd) {
    int err = 0;
    char* html_header = read_header(fd);
    char* get_header = parse_get_header(html_header);

    printf("%s\n", get_header);

    err = send(fd, html_header, strlen(html_header), 0);
    if (err == -1) {
        perror("send() failed");
    }

    err = shutdown(fd, SHUT_RDWR);
    if (err == -1) {
        perror("shutdown() failed");
    }
}

char* read_header(int fd) {
    char* header = malloc(sizeof(char) * MAX_HEADER_SIZE);

    ssize_t now_read = read(fd, header, MAX_HEADER_SIZE);
    if (now_read == 0) {
        header[0] = '\0';
    }

    return header;
}
