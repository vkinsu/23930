#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <errno.h>

#define SOCKET_PATH "./upper_case_socket"
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024
#define EXIT_COMMAND "exit"

void to_upper_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int server_fd, epoll_fd;
    struct sockaddr_un server_addr;
    int running = 1;

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


    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }


    struct epoll_event event, events[MAX_EVENTS];
    event.events = EPOLLIN;
    event.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl: server_fd");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s...\n", SOCKET_PATH);

    while (running) {
        
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < num_fds; i++) {
            int fd = events[i].data.fd;


            if (fd == server_fd) {
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }
                printf("New client connected (fd=%d)\n", client_fd);


                event.events = EPOLLIN | EPOLLET; 
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror("epoll_ctl: client_fd");
                    close(client_fd);
                }
            }
            
            else {
                char buffer[BUFFER_SIZE];
                ssize_t num_bytes = read(fd, buffer, sizeof(buffer) - 1);

                if (num_bytes == 0) {
                    
                    printf("Client disconnected (fd=%d)\n", fd);
                    close(fd);
                } else if (num_bytes > 0) {
                    buffer[num_bytes] = '\0';


                    if (strcmp(buffer, EXIT_COMMAND) == 0) {
                        printf("Exit command received. Shutting down server...\n");
                        running = 0;
                        break;
                    }

                    to_upper_case(buffer);
                    printf("Converted text (fd=%d): %s\n", fd, buffer);
                } else if (num_bytes == -1 && errno != EAGAIN) {
                    perror("read");
                    close(fd);
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    unlink(SOCKET_PATH);

    printf("Server terminated.\n");
    return 0;
}

