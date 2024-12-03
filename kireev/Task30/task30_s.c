#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[S]Server is listening on %s\n", SOCKET_PATH);

    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t num_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (num_read <= 0) {
            break;
        }
        buffer[num_read] = '\0';

        to_uppercase(buffer);
        printf("[S]Received and converted to uppercase: %s\n", buffer);
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
	printf("[S]Server stoped!\n");
    exit(0);
}
