#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 2048
#define SPAM_DELAY 1 // Задержка между сообщениями в секундах

int main() {
    int client_fd;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];
    int message_count = 1;

    // Создаем сокет
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Указываем адрес сервера
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    // Подключаемся к серверу
    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Spammer client connected to the server.\n");

    while (1) {
        // Генерируем сообщение
        snprintf(buffer, BUFFER_SIZE, "Spam message #%d", message_count++);
        
        // Отправляем сообщение на сервер
        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }

        printf("Sent: %s\n", buffer);

        // Задержка перед следующим сообщением
        sleep(SPAM_DELAY);
    }

    close(client_fd);
    return 0;
}
