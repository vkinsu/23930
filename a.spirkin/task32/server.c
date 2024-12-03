#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <ctype.h>
#include <pthread.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 1024

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    char buffer[1024];
    ssize_t bytes_received;

    while ((bytes_received = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';

        for (int j = 0; buffer[j] != '\0'; j++) {
            buffer[j] = toupper(buffer[j]);
        }

        printf("Получено сообщение: %s", buffer);
    }

    close(client_fd);
    free(arg);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    fd_set read_fds;
    int max_fd;
    char buffer[BUFFER_SIZE];
    socklen_t client_len;
    pthread_t client_thread;
    struct sockaddr_un client_address;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("\nsocket\n");
        exit(1);
    }

    unlink(SOCKET_PATH);

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен и слушает на %s\n", SOCKET_PATH);

    while (1) {
        client_len = sizeof(client_address);
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        if (pthread_create(&client_thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);
        } else {
            pthread_detach(client_thread);
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}