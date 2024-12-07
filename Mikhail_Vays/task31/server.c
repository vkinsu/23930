#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <ctype.h>

#define SOCKET_PATH "./upper_case_socket"
#define BUFFER_SIZE 256

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds, master_fds;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);
    max_fd = server_fd;

    printf("Server is waiting for connections...\n");

    while (1) {
        read_fds = master_fds; // Copy 'master_fds' in 'read_fds' for 'select'

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_fd) {
                    client_fd = accept(server_fd, NULL, NULL);
                    if (client_fd == -1) {
                        perror("accept");
                        continue;
                    }

                    FD_SET(client_fd, &master_fds);
                    if (client_fd > max_fd) {
                        max_fd = client_fd;
                    }

                    printf("New client connected\n");
                } else {
                    ssize_t bytes_read = read(i, buffer, sizeof(buffer) - 1);
                    if (bytes_read <= 0) {
                        if (bytes_read == 0) {
                            printf("Client disconnected\n");
                        } else {
                            perror("read");
                        }
                        close(i);
                        FD_CLR(i, &master_fds);
                    } else {
                        buffer[bytes_read] = '\0';
                        to_uppercase(buffer);
                        printf("Received from client: %s\n", buffer);
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    printf("Server termination.\n");
    return 0;
}
