#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket making failed\n");
        exit(1);
    }
    unlink(SOCKET_PATH);
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Server connection failed\n");
        close(server_fd);
        exit(1);
    }
    if (listen(server_fd, 5) == -1) {
        perror("Socket listening failed\n");
        close(server_fd);
        exit(1);
    }
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("Accept failed\n");
        close(server_fd);
        exit(1);
    }
    while (1) {
        ssize_t read_bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (read_bytes <= 0) {
            break;
        }
        buffer[read_bytes] = '\0';
        for (int i = 0; buffer[i]; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }
        printf("input in upper case: %s\n", buffer);
    }
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
