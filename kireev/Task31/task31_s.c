#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define SOCKET_PATH "/tmp/unix_socket_test"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds, temp_fds;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[S]Server is listening on %s\n", SOCKET_PATH);

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    while (1) {
        temp_fds = read_fds;

        if (select(max_fd + 1, &temp_fds, NULL, NULL, NULL) == -1) {
            perror("Select failed");
            close(server_fd);
            unlink(SOCKET_PATH);
            exit(EXIT_FAILURE);
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &temp_fds)) {
                if (fd == server_fd) {
                    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                        perror("Accept failed");
                    } else {
                        FD_SET(client_fd, &read_fds);
                        if (client_fd > max_fd) {
                            max_fd = client_fd;
                        }
                        printf("[S]New client connected fd: %d\n", client_fd);
                    }
                } else {
                    ssize_t num_read = read(fd, buffer, sizeof(buffer) - 1);
                    if (num_read <= 0) {
                        printf("[S]Client disconnected fd: %d\n", fd);
                        close(fd);
                        FD_CLR(fd, &read_fds);
                    } else {
                        buffer[num_read] = '\0';
                        to_uppercase(buffer);
                        printf("[S]Received from %d and converted to uppercase: %s\n", fd, buffer);
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    printf("[S]Server stopped!\n");
    exit(0);
}
