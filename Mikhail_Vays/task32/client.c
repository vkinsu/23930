#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./upper_case_socket"
#define BUFFER_SIZE 256

int main() {
    int client_fd;
    struct sockaddr_un server_addr;

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
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

    printf("Enter initial message to send to server ('exit' to quit):\n");
    char buffer[BUFFER_SIZE];

    printf("> ");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        close(client_fd);
        return 0;
    }

    // Remove newline
    buffer[strcspn(buffer, "\n")] = '\0';

    if (strcmp(buffer, "exit") == 0) {
        close(client_fd);
        return 0;
    }

    // Initial send
    if (write(client_fd, buffer, strlen(buffer)) == -1) {
        perror("write");
        close(client_fd);
        return 0;
    }

    while (1) {
        // Receive response
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("Server disconnected.\n");
            } else {
                perror("read");
            }
            break;
        }
        buffer[bytes_read] = '\0';

        printf("Server: %s\n", buffer);

        // Send response back
        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("write");
            break;
        }
    }

    close(client_fd);
    return 0;
}
