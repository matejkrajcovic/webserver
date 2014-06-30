#include "server.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define BACK_LOG 10 // number of waiting clients
#define MAX_HEADER_SIZE 2048 // maximum size of html header client sends

int fd;

void start_server(StartupArguments* arguments) {
    int err = 0;
    int newfd;
    struct sockaddr_in me, peer;
    socklen_t peerlen;

    signal(SIGINT, interrupt_signal_handler);
    signal(SIGCHLD, child_signal_handler);

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

        err = fork();
        if (err == -1) {
            perror("fork() failed");
            exit(1);
        } else if (err == 0) { // child
            err = close(fd);
            if (err == -1) {
                perror("close() failed");
            }

            handle_client(newfd);

            err = shutdown(newfd, SHUT_RDWR);
            if (err == -1) {
                perror("shutdown() failed");
            }

            while (read(newfd, NULL, 1024) > 0);

            err = close(newfd);
            if (err == -1) {
                perror("close() failed");
            }
            printf("Connection closed.\n");

            exit(0);
        } else { // parent
            err = close(newfd);
            if (err == -1) {
                perror("close() failed");
            }
        }
    }
}

void handle_client(int fd) {
    int err = 0;
    char* html_header = read_header(fd);
    char* get_header = parse_get_header(html_header);

    if (html_header == NULL) {
        return;
    }

    if (strstr(get_header, "/../") != NULL) {
        err = send(fd, "Do not use '/../' in path.\n", 25, 0);
        if (err == -1) {
            perror("send() failed");
        }
        return;
    }

    char* file_name = get_header + 1;

    send_file(fd, file_name);

    free(html_header);
}

char* read_header(int fd) {
    char* header = malloc(sizeof(char) * MAX_HEADER_SIZE);
    if (header == NULL) {
        perror("malloc() failed");
        return NULL;
    }

    ssize_t now_read = read(fd, header, MAX_HEADER_SIZE);
    if (now_read == -1) {
        header[0] = '\0';
        perror("read() failed");
    } else {
        header[now_read] = '\0';
    }

    return header;
}

void send_file(int fd, char* file_name) {
    int err = 0;
    struct stat file_stats;
    int file_size;
    void* file_content;
    ssize_t file_content_size;
    char content_length_header[1024];

    err = stat(file_name, &file_stats); // needed to find out file's size
    if (err == -1) {
        perror("stat() failed");
        if (errno == EACCES) {
            err = send(fd, "File does not exist.", 20, 0);
            if (err == -1) {
                perror("send() failed");
            }
        } else {
            err = send(fd, "Reading file failed.", 20, 0);
            if (err == -1) {
                perror("send() failed");
            }
        }
        return;
    }

    file_size = file_stats.st_size;
    file_content = malloc(sizeof(char) * file_size);
    if (file_content == NULL) {
        perror("malloc() failed");
        return;
    }

    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1) {
        perror("open() failed");
        free(file_content);
        return;
    }

    file_content_size = read(file_fd, file_content, file_size);
    if (file_content_size == -1) {
        perror("read() failed");
        free(file_content);
        return;
    }

    err = send(fd, "HTTP/1.0 200 OK\nContent-Type: text/html; charset=UTF-8\n", 56, 0);
    if (err == -1) {
        perror("send() failed");
        free(file_content);
        return;
    }

    snprintf(content_length_header, sizeof(content_length_header),
        "Content-Length: %zd\n\n", file_content_size);
    err = send(fd, content_length_header, strlen(content_length_header), 0);
    if (err == -1) {
        perror("send() failed");
        free(file_content);
        return;
    }

    printf("Sending %s\n", file_name);
    err = send(fd, file_content, file_content_size, 0);
    if (err == -1) {
        perror("send() failed");
        free(file_content);
        return;
    }

    free(file_content);
}

void interrupt_signal_handler(int a) {
    printf("\nShuting down...");

    int err = shutdown(fd, SHUT_RDWR);
    if (err == -1) {
        perror("shutdown() failed");
    }

    while (read(fd, NULL, 1024) > 0);

    err = close(fd);
    if (err == -1) {
        perror("close() failed");
    }

    exit(0);
}

void child_signal_handler(int a) {
    while(waitpid(-1, NULL, WNOHANG) >= 0);
    signal(SIGCHLD, child_signal_handler);
}
