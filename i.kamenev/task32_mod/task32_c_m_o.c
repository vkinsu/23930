#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./upper_case_socket"
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_un server_addr;

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        ssize_t num_bytes = read(sock_fd, buffer, sizeof(buffer) - 1);
        if (num_bytes > 0) {
            buffer[num_bytes] = '\0';
            printf("Message from server: %s\n", buffer);
        } else {
            break;
        }
    }

    close(sock_fd);
    return 0;
}
