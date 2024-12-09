#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("\nSocket making error\n");
        exit(1);
    }
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("\nConnection error\n");
        close(client_fd);
        exit(1);
    }
    while (1) {
        bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_received == -1) {
            perror("\nReading from server error");
            close(client_fd);
            exit(1);
        }
        if (bytes_received == 0) {
            printf("\nServer closed the connection\n");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
        if (write(client_fd, "str3", 4) == -1) {
            perror("\nWriting error");
            close(client_fd);
            exit(1);
        }
    }
    close(client_fd);
    return 0;
}

