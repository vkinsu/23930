#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <errno.h>

#define SOCKET_PATH "unix_socket"
#define MAX_EVENTS 10
#define BUFFER_SIZE 2048

// Конвертирует строку в верхний регистр
void to_upper_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// Создаёт и настраивает серверный сокет
int setup_server_socket() {
    int server_fd;
    struct sockaddr_un server_addr;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server socket created and listening on %s\n", SOCKET_PATH);
    return server_fd;
}

// Настраивает epoll
int setup_epoll(int server_fd) {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl: server_fd");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    return epoll_fd;
}

// Обрабатывает подключение нового клиента
void handle_new_client(int server_fd, int epoll_fd) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        return;
    }

    printf("New client connected (fd=%d)\n", client_fd);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        perror("epoll_ctl: client_fd");
        close(client_fd);
    }
}

// Обрабатывает данные от клиента
int handle_client_data(int client_fd, int epoll_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t num_bytes = read(client_fd, buffer, sizeof(buffer) - 1);

    if (num_bytes == 0) {
        printf("Client disconnected (fd=%d)\n", client_fd);
        close(client_fd);
        return 0;
    } else if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        printf("Received from fd=%d: %s\n", client_fd, buffer);

        // Преобразовать текст в верхний регистр
        to_upper_case(buffer);

        // Добавить информацию об отправителе
        char message_with_sender[BUFFER_SIZE];
        snprintf(message_with_sender, sizeof(message_with_sender), "From client %d: %s", client_fd, buffer);

        // Переслать сообщение всем остальным клиентам
        struct epoll_event events[MAX_EVENTS];
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, 0);
        for (int i = 0; i < num_fds; i++) {
            int target_fd = events[i].data.fd;
            if (target_fd != client_fd && target_fd != -1) { // Не отправляем самому себе или серверу
                if (write(target_fd, message_with_sender, strlen(message_with_sender)) == -1) {
                    perror("write");
                }
            }
        }
    } else if (num_bytes == -1 && errno != EAGAIN) {
        perror("read");
        close(client_fd);
    }

    return 1;
}

// Основной цикл сервера
void run_server(int server_fd, int epoll_fd) {
    struct epoll_event events[MAX_EVENTS];
    int running = 1;

    while (running) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < num_fds; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                handle_new_client(server_fd, epoll_fd);
            } else {
                handle_client_data(fd, epoll_fd);
            }
        }
    }
}

// Завершает работу сервера и освобождает ресурсы
void cleanup(int server_fd, int epoll_fd) {
    close(server_fd);
    close(epoll_fd);
    unlink(SOCKET_PATH);
    printf("Server terminated.\n");
}

int main() {
    int server_fd = setup_server_socket();
    int epoll_fd = setup_epoll(server_fd);

    run_server(server_fd, epoll_fd);

    cleanup(server_fd, epoll_fd);
    return 0;
}

