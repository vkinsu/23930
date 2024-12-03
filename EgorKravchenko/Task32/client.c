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
    if((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    if (connect(client_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Connection failed\n");
        close(client_fd);
        exit(1);
    }
    while (fgets(buffer, sizeof(buffer), stdin)) {
        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("Writing failed\n");
            close(client_fd);
            exit(1);
        }
    }
    close(client_fd);
    return 0;
}
