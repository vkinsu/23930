#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <sys/select.h>

#define SOCKET_PATH "./unix_socket"
#define MAX_CLIENTS 10

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    fd_set read_fds;
    int max_fd;
    int client_fds[MAX_CLIENTS];
    int num_clients = 0;
    char buffer[1024];
    int bytes_received;
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH);
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    if (listen(server_sock, 1) == -1) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on %s\n", SOCKET_PATH);
    FD_ZERO(&read_fds);
    FD_SET(server_sock, &read_fds);
    max_fd = server_sock;
    while (1) {
        fd_set tmp_fds = read_fds;
        int activity = select(max_fd + 1, &tmp_fds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select");
            close(server_sock);
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(server_sock, &tmp_fds)) {
            client_len = sizeof(client_addr);
            client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0) {
                perror("accept");
                close(server_sock);
                exit(EXIT_FAILURE);
            }
            FD_SET(client_sock, &read_fds);
            if (client_sock > max_fd) {
                max_fd = client_sock;
            }
            client_fds[num_clients++] = client_sock;
           //printf("New client connected: %d\n", client_sock);
        }
        for (int i = 0; i < num_clients; i++) {
            client_sock = client_fds[i];

            if (FD_ISSET(client_sock, &tmp_fds)) {
                bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) {
                    close(client_sock);
                    FD_CLR(client_sock, &read_fds);
                    client_fds[i] = -1;
                   // printf("Client disconnected: %d\n", client_sock);
                } else {
                    for (int j = 0; j < bytes_received; j++) {
                        buffer[j] = toupper(buffer[j]);
                        printf("%c", buffer[j]);
                    }
                }
            }
        }
    }
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
