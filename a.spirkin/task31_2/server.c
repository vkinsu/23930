#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define SOCKET_PATH "./socket"
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    fd_set read_fds, master_fds;
    int max_fd;
    int client_sockets[MAX_CLIENTS] = {0};
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("\nsocket\n");
        exit(1);
    }

    unlink(SOCKET_PATH);

    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);
    max_fd = server_fd;

    while (1) {
        read_fds = master_fds;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                perror("accept");
                continue;
            }

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_fd;
                    FD_SET(client_fd, &master_fds);
                    if (client_fd > max_fd) {
                        max_fd = client_fd;
                    }
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0 && FD_ISSET(client_sockets[i], &read_fds)) {
                ssize_t bytes_read = read(client_sockets[i], buffer, BUFFER_SIZE);
                if (bytes_read <= 0) {
                    if (bytes_read == 0) {
                        printf("Client disconnected\n");
                    } else {
                        perror("read");
                    }
                    close(client_sockets[i]);
                    FD_CLR(client_sockets[i], &master_fds);
                    client_sockets[i] = 0;
                } else {
                    buffer[bytes_read] = '\0';
                    printf("Received: %s\n", buffer);

                    if (write(client_sockets[1], buffer, strlen(buffer)) == -1) {
                        perror("write");
                        break;
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}