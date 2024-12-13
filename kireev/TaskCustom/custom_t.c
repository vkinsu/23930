#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_socket_test"
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("[T] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("[T] Connection to server failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[T] Enter text to send to the server (CTRL+D to stop):\n");

    while (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        if (write(sock_fd, buffer, strlen(buffer)) == -1) {
            perror("[T] Write to socket failed");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(sock_fd);
    printf("[T] Transmitter terminated.\n");
    exit(0);
}
