#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

void usage(char* argv[]);
void build_server(const char*);
void socket_error_exit(const char*);

int main(int argc, char* argv[]) {
    if (argc == 1) {
        build_server("");
    } else if (argc == 2) {
        build_server(argv[1]);
    } else {
        usage(argv);
    }
    return 0; 
}

void build_server(const char* port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        socket_error_exit("fail to construct socket");
    }

    int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
        socket_error_exit("fail to setsockopt");
    }

    // TODO
}

void socket_error_exit(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void usage(char* argv[]) {
    printf("Usage: %s (port)\n", argv[1]);
    exit(EXIT_FAILURE);
}