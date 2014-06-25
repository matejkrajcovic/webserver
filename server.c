#include "server.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


#define BACK_LOG 10 // number of waiting clients

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
    peerlen = sizeof(peer);
    newfd = accept(fd, (struct sockaddr*) &peer, &peerlen);
    if (newfd == -1) {
        perror("accept() failed");
    }

    err = send(newfd, "Test message\n", 13, 0);
    if (err == -1) {
        perror("send() failed");
    }

    err = shutdown(newfd, SHUT_RDWR);
    if (err == -1) {
        perror("shutdown() failed");
    }

    err = close(newfd);
    if (err == -1) {
        perror("close() failed");
    }

    err = close(fd);
    if (err == -1) {
        perror("close() failed");
    }

}
