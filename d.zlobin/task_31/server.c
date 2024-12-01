#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <fcntl.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 2048
#define MAX_CLIENTS 10

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un address;
    fd_set active_fds, read_fds;
    char buffer[BUFFER_SIZE];
    int client_sockets[MAX_CLIENTS] = {0};
    int addr_len = sizeof(address);

    // socket creating:
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // deleting old socket if it exists:
    unlink(SOCKET_PATH);

    // Настройка адреса
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    // socket connecting:
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // connections listening:
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // realization of mant descriptors:
    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);
    max_fd = server_fd;

    printf("Server listening on %s\n", SOCKET_PATH);

    while (1) {
        read_fds = active_fds;

	// waiting for activity on one of the sockets:
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

	// checking of any activity on server socket:
        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_len)) == -1) {
                perror("Accept failed");
                continue;
            }

            printf("New client connected: %d\n", client_fd);

	    // adding a new client in list:
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_fd;
                    FD_SET(client_fd, &active_fds);
                    if (client_fd > max_fd) max_fd = client_fd;
                    break;
                }
            }
        }

	// checking of clients sockets' activities 
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_sockets[i];

            if (FD_ISSET(sock, &read_fds)) {
                int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);

                if (bytes_read <= 0) {
                    // client closed the connection:
                    printf("Client disconnected: %d\n", sock);
                    close(sock);
                    FD_CLR(sock, &active_fds);
                    client_sockets[i] = 0;
                } else {
                    buffer[bytes_read] = '\0';
                    to_uppercase(buffer);
                    printf("Client %d: %s\n", sock, buffer);
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
