#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h> 

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int server_fd, new_client_fd, max_fd, activity;
    int client_sockets[MAX_CLIENTS];
    fd_set read_fds;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Инициализация массива клиентских сокетов
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }

    // Создание сокета
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Ожидание соединений
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер ожидает соединений на порту %d...\n", PORT);

    while (1) {
        // Очистка и настройка набора файловых дескрипторов
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;

        // Добавляем активные клиентские сокеты
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
            }
            if (client_sockets[i] > max_fd) {
                max_fd = client_sockets[i];
            }
        }

        // Используем select для мультиплексирования
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            perror("select");
            break;
        }

        // Обработка нового входящего соединения
        if (FD_ISSET(server_fd, &read_fds)) {
            new_client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
            if (new_client_fd < 0) {
                perror("accept");
                continue;
            }

            printf("Новый клиент подключен: сокет %d\n", new_client_fd);

            // Добавляем нового клиента в массив
            int added = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == -1) {
                    client_sockets[i] = new_client_fd;
                    added = 1;
                    break;
                }
            }

            if (!added) {
                printf("Максимальное количество клиентов достигнуто.\n");
                close(new_client_fd);
            }
        }

        // Обработка данных от клиентов
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int client_fd = client_sockets[i];

            if (client_fd > 0 && FD_ISSET(client_fd, &read_fds)) {
                char buffer[BUFFER_SIZE];
                ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

                if (bytes_read <= 0) {
                    // Если клиент отключился
                    if (bytes_read == 0) {
                        printf("Клиент сокет %d отключился.\n", client_fd);
                    } else {
                        perror("read");
                    }
                    close(client_fd);
                    client_sockets[i] = -1;  // Удаляем сокет из массива
                } else {
                    // Обрабатываем данные
                    buffer[bytes_read] = '\0';
                    to_uppercase(buffer);
                    printf("Клиент %d: %s\n", client_fd, buffer);
                }
            }
        }
    }

    // Закрытие всех сокетов
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] > 0) {
            close(client_sockets[i]);
        }
    }
    close(server_fd);
    return 0;
}
