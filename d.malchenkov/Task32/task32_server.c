#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Функция для преобразования строки в верхний регистр
void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Структура для передачи информации в поток
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} client_info_t;

// Функция обработки клиента
void* handle_client(void* arg) {
    client_info_t *client_info = (client_info_t*)arg;
    int client_fd = client_info->client_fd;
    char client_ip[INET_ADDRSTRLEN];

    // Получение IP-адреса клиента
    inet_ntop(AF_INET, &(client_info->client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Клиент подключен: %s:%d (сокет %d)\n",
           client_ip,
           ntohs(client_info->client_addr.sin_port),
           client_fd);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Чтение данных от клиента
    while ((bytes_read = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // Завершение строки
        to_uppercase(buffer);
        printf("Клиент %d: %s\n", client_fd, buffer);
    }

    if (bytes_read == -1) {
        perror("read");
    } else {
        printf("Клиент %d отключился.\n", client_fd);
    }

    close(client_fd);
    free(client_info);
    pthread_exit(NULL);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;

    // Создание сокета
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Любой доступный интерфейс
    server_addr.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Прослушивание входящих соединений
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер ожидает соединений на порту %d...\n", PORT);

    while (1) {
        // Принятие нового соединения
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        // Выделение памяти для информации о клиенте
        client_info_t *client_info = malloc(sizeof(client_info_t));
        if (!client_info) {
            perror("malloc");
            close(client_fd);
            continue;
        }

        client_info->client_fd = client_fd;
        memcpy(&client_info->client_addr, &client_addr, sizeof(client_addr));

        // Создание нового потока для обработки клиента
        if (pthread_create(&tid, NULL, handle_client, (void*)client_info) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(client_info);
            continue;
        }

        // Отсоединение потока, чтобы освободить ресурсы после завершения
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
