#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 1024

void send_message(int sock_fd, const char *message) {
    if (write(sock_fd, message, strlen(message)) == -1) {
        perror("Failed to send message");
        exit(EXIT_FAILURE);
    }
    usleep(1000); 
}

int main() {
    int sock_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    int counter = 0;

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Failed to connect to the server.");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    while (counter < 1024) {
        send_message(sock_fd, "client");
        send_message(sock_fd, "isright");

        ssize_t bytes_read = read(sock_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Server response: %s\n", buffer);

            sscanf(buffer, "client isright%d", &counter);
        } else {
            perror("Failed to read server response");
            break;
        }
    }

    printf("Counter reached %d. Disconnecting...\n", counter);
    close(sock_fd);
    return 0;
}
