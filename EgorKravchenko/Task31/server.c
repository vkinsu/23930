#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <fcntl.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int main() {
    int server_fd, client_fd, max_fd, activity;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];
    fd_set read_fds, temp_fds;
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    unlink(SOCKET_PATH);
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;
    while (1) {
        temp_fds = read_fds;
        activity = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
            break;
        }
        if (FD_ISSET(server_fd, &temp_fds)) {
            client_fd = accept(server_fd, NULL, NULL);
            if (client_fd == -1) {
                perror("Accept failed");
            } else {
                printf("Client %d connected\n", client_fd);
                FD_SET(client_fd, &read_fds);
                if (client_fd > max_fd) {
                    max_fd = client_fd;
                }
            }
        }
        for (int i = 0; i <= max_fd; i++) {
            if (i != server_fd && FD_ISSET(i, &temp_fds)) {
                ssize_t read_bytes = read(i, buffer, BUFFER_SIZE - 1);
                if (read_bytes <= 0) {
                    printf("Client %d disconnected\n", i);
                    close(i);
                    FD_CLR(i, &read_fds);
                } else {
                    buffer[read_bytes] = '\0';
                    for (int j = 0; buffer[j]; j++) {
                        buffer[j] = toupper((unsigned char)buffer[j]);
                    }
                    printf("Client %d input in upper case: %s", i, buffer);
                }
            }
        }
    }
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

