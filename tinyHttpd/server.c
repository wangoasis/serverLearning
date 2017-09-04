#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#define ISspace(x) isspace((int)x)

void usage(char**);
void build_server(const char*);
void socket_error_exit(const char*);
void* accept_request(void*);
int get_line(int, char*, int);
void not_found(int);
void execute_cgi(int, const char*, const char*, const char*);
void serve_file(int, const char*);

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
    // int setsockopt(int sockfd, int level, int option, const void *val, socklen_t len);
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
        socket_error_exit("fail to setsockopt");
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    if (strlen(port) == 0) {
        // dynamically allocate a port
        server.sin_port = htons(0);
    } else {
        server.sin_port = htons((unsigned int)atoi(port));
    }

    // int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        socket_error_exit("fail to bind");
    }

    socklen_t len = sizeof(server);
    // int getsockname(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict alenp);
    if (getsockname(sock, (struct sockaddr*)&server, &len) == -1) {
        socket_error_exit("fail to getsockname");
    }

    if (listen(sock, 10) != 0) {
        socket_error_exit("fail to listen");
    }

    printf("@@@port %hu\n", ntohs(server.sin_port));

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t newthread;
    while (1) {
        // int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict len);
        int accept_id = accept(sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (accept_id == -1) {
            socket_error_exit("fail to accept");
        }
        if (pthread_create(&newthread , NULL, (void *)accept_request, (void *)(intptr_t)accept_id) != 0)
            socket_error_exit("fail to pthread_create");
    }

    close(sock);
}

void socket_error_exit(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void usage(char* argv[]) {
    printf("Usage: %s (port)\n", argv[0]);
    exit(EXIT_FAILURE);
}

void* accept_request(void* arg) {
    int client = (intptr_t)arg;
    char buf[1024];
    char method[255]; // POST or GET
    char url[255]; 
    char path[255];
    size_t numchars;
    size_t i = 0, j = 0;
    int cgi = 0; // flag for executing cgi file
    char* query_string = NULL; // used to get content after "?" in GET method
    struct stat st;

    // read first line of request header
    numchars = get_line(client, buf, sizeof(buf));
    // get method name: POST or GET
    while (i < numchars && !ISspace(buf[i]) && j < sizeof(method)) {
        method[j++] = buf[i++];
    }
    method[j] = '\0';

    if (strcasecmp(method, "POST") == 0) {
        cgi = 1;
    }

    // skip space between method and url
    while (i < numchars && ISspace(buf[i]))
        i++;
    // get url
    j = 0;
    while (i < numchars && !ISspace(buf[i]) && j < sizeof(url)) {
        url[j++] = buf[i++];
    }
    url[j] = '\0';

    // get query_string
    if (strcasecmp(method, "GET") == 0) {
        query_string = url;
        while (*query_string != '?' && *query_string != '\0')
            query_string++;
        if (*query_string == '?') {
            cgi = 1; // need to execute cgi file
            *query_string = '\0'; // now url will not contain '?'
            query_string++;
        }
    }

    sprintf(path, "htdocs%s", url);

    // if path is a directory, the set dafault page: index.html
    if (path[strlen(path)-1] == '/')
        strcat(path, "index.html");
    // int stat(const char *file_name, struct stat *buf)
    // access file infomation specified by file_name, saved in struct stat
    if (stat(path, &st) == -1) {
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numchars = get_line(client, buf, sizeof(buf));
        // page not exist
        not_found(client);
    } else {
        if ((st.st_mode & S_IFMT) == S_IFDIR)
            strcat(path, "/index.html");
        if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) 
            cgi = 1;
        if (cgi) {
            execute_cgi(client, path, method, query_string);
        } else {
            serve_file(client, path);
        }
    }

    close(client);
    return (void*)0;
}

// no matter a line of header ends at '\r', '\n', or '\r\n'
// substitute the end line to '\n'
int get_line(int sock, char* buf, int size) {
    int nbytes = 0;
    char single_buf[1]; // used for receive one character
    int n;

    while (nbytes < size - 1 && single_buf[0] != '\n') {
        n = recv(sock, single_buf, 1, 0);
        if (n > 0) {
            if (single_buf[0] == '\r') {
                // When we specify the MSG_PEEK flag, 
                // we can peek at the next data to be read without actually consuming it.
                n = recv(sock, single_buf, 1, MSG_PEEK);
                if (n > 0 && single_buf[0] == '\n') 
                    n = recv(sock, single_buf, 1, 0);
                else 
                    single_buf[0] = '\n'; 
            }
            buf[nbytes++] = single_buf[0];
        } else {
            single_buf[0] = '\n';
        }
    }
    buf[nbytes] = '\n';
    return nbytes;
}

void not_found(int client) {
    // TODO
}

void execute_cgi(int client, const char* path, const char* method, const char* query_string) {
    // TODO
}

void serve_file(int client, const char* path) {
    // TODO
}