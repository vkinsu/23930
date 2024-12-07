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

    printf("Enter message to send on server ('exit' for closing the connection):\n");
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("> ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break;
        }

        // Removing newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("write");
            break;
        }
    }

    close(client_fd);
    printf("Client termination.\n");
    return 0;
}
