#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <ctype.h>

#define SOCKET_PATH "./upper_case_socket"
#define MAX_CLIENTS 10

void to_upper_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un server_addr;
    fd_set read_fds;
    char buffer[1024];
    int client_fds[MAX_CLIENTS] = {0};

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
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    max_fd = server_fd;

    printf("Server is listening on %s...\n", SOCKET_PATH);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] != 0) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd) {
                    max_fd = client_fds[i];
                }
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                perror("accept");
                continue;
            }

            printf("New client connected\n");

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == 0) {
                    client_fds[i] = client_fd;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] != 0 && FD_ISSET(client_fds[i], &read_fds)) {
                ssize_t num_bytes = read(client_fds[i], buffer, sizeof(buffer) - 1);
                if (num_bytes <= 0) {

                    close(client_fds[i]);
                    client_fds[i] = 0;
                    printf("Client disconnected\n");
                } else {
                    buffer[num_bytes] = '\0';
                    
                    to_upper_case(buffer);
                    printf("Converted text: %s\n", buffer);
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}

