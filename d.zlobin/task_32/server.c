#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <errno.h>

#define SOCKET_PATH "unix_socket"
#define MAX_EVENTS 10
#define BUFFER_SIZE 2048
#define EXIT_COMMAND "exit"

// convert a string to uppercase:
void to_upper_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// create and set up a Unix domain socket:
int setup_server_socket() {
    int server_fd;
    struct sockaddr_un server_addr;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server socket created and listening on %s\n", SOCKET_PATH);
    return server_fd;
}

// set up epoll instance and add the server socket:
int setup_epoll(int server_fd) {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl: server_fd");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    printf("Epoll instance created and server socket added.\n");
    return epoll_fd;
}

// accept a new client connection and add it to epoll:
void handle_new_client(int server_fd, int epoll_fd) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        return;
    }

    printf("New client connected (fd=%d)\n", client_fd);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        perror("epoll_ctl: client_fd");
        close(client_fd);
    }
}

// handle data from a connected client:
int handle_client_data(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t num_bytes = read(client_fd, buffer, sizeof(buffer) - 1);

    if (num_bytes == 0) {
        printf("Client disconnected (fd=%d)\n", client_fd);
        close(client_fd);
        return 0;
    } else if (num_bytes > 0) {
        buffer[num_bytes] = '\0';

        if (strcmp(buffer, EXIT_COMMAND) == 0) {
            printf("Exit command received. Shutting down server...\n");
            return -1;
        }

        to_upper_case(buffer);
        printf("Converted text (fd=%d): %s\n", client_fd, buffer);
    } else if (num_bytes == -1 && errno != EAGAIN) {
        perror("read");
        close(client_fd);
    }

    return 1;
}

// main server loop:
void run_server(int server_fd, int epoll_fd) {
    struct epoll_event events[MAX_EVENTS];
    int running = 1;

    while (running) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < num_fds; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                handle_new_client(server_fd, epoll_fd);
            } else {
                int result = handle_client_data(fd);
                if (result == -1) { // Exit command received
                    running = 0;
                    break;
                }
            }
        }
    }
}

// cleanup resources:
void cleanup(int server_fd, int epoll_fd) {
    close(server_fd);
    close(epoll_fd);
    unlink(SOCKET_PATH);
    printf("Server terminated.\n");
}

int main() {
    int server_fd = setup_server_socket();
    int epoll_fd = setup_epoll(server_fd);

    run_server(server_fd, epoll_fd);

    cleanup(server_fd, epoll_fd);
    return 0;
}
