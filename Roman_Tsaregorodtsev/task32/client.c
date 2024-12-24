#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_socket"

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[1024];

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        //printf("Enter text to send to the server: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("write");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(client_fd);
    return 0;
}
