#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 256

int main() {
    int client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Введите текст для отправки серверу (Ctrl+D для завершения):\n");

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("write");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(client_fd);

    printf("Клиент завершил работу.\n");
    return 0;
}
