#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define SOCKET_PATH "./unix_domain_socket"

int main() {
  int client_fd;
  struct sockaddr_un address;
  char buffer[1024];
  fd_set read_fds;
  int max_fd;

  // Создаем сокет
  client_fd = socket(AF_UNIX, SOCK_STREAM, 0); // потоковый сокет
  if (client_fd == -1) {
    perror("\nSocket making error\n");
    exit(1);
  }

  // Настраиваем адрес сокета
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

  // Устанавливаем соединение с сервером
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("\nConnection error\n");
    close(client_fd);
    exit(1);
  }

  max_fd = client_fd > STDIN_FILENO ? client_fd : STDIN_FILENO;

  while (1) {
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(client_fd, &read_fds);

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("\nSelect error\n");
      close(client_fd);
      exit(1);
    }

    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
      // Читаем данные из стандартного ввода
      if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        perror("\nReading from stdin error\n");
        close(client_fd);
        exit(1);
      }

      // Отправляем текст серверу
      if (write(client_fd, buffer, strlen(buffer)) == -1) {
        perror("\nWriting error");
        close(client_fd);
        exit(1);
      }
    }

    if (FD_ISSET(client_fd, &read_fds)) {
      // Читаем данные от сервера
      ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
      if (bytes_received <= 0) {
        if (bytes_received == -1) {
          perror("\nReading from server error\n");
        }
        close(client_fd);
        exit(1);
      } else {
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
      }
    }
  }

  // Закрываем сокет
  close(client_fd);

  return 0;
}
