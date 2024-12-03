#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 2048

int main() {
    int client_fd;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup the address
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server. Type messages to send:\n");

    while (1) {
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove newline character
        buffer[strcspn(buffer, "\n")] = '\0';

        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    close(client_fd);
    return 0;
}
