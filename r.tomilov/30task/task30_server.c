#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 256

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер ожидает подключения...\n");

    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Клиент подключен. Ожидание данных...\n");

    ssize_t num_read;
    while ((num_read = read(client_fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[num_read] = '\0';
        to_uppercase(buffer);
        printf("Получено и преобразовано: %s\n", buffer);
    }

    if (num_read == -1) {
        perror("read");
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    printf("Сервер завершил работу.\n");
    return 0;
}