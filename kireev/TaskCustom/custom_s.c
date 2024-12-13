#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <pthread.h>

#define SOCKET_PATH "/tmp/unix_socket_test"
#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

void *handle_receiver(void *arg) {
    int receiver_fd = *(int *)arg;
    char message[] = "SERVERTESTMESSAGE";
    while (1) {
        if (write(receiver_fd, message, strlen(message)) == -1) {
            perror("Failed to send message to receiver");
            break;
        }
        sleep(1);
    }
    close(receiver_fd);
    return NULL;
}

int main() {
    int server_fd, transmitter_fd, receiver_fd;
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

    if (listen(server_fd, 2) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[S]Waiting for receiver to connect...\n");
    if ((receiver_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("Failed to accept receiver");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("[S]Receiver connected.\n");

    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, handle_receiver, &receiver_fd);

    while (1) {
        printf("[S]Waiting for transmitter to connect...\n");
        if ((transmitter_fd = accept(server_fd, NULL, NULL)) == -1) {
            perror("Failed to accept transmitter");
            continue;
        }

        printf("[S]Transmitter connected.\n");

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t bytes_read = read(transmitter_fd, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) {
				printf("[S]Transmitter disconnected!\n");
                close(transmitter_fd);
                break;
            }

            to_uppercase(buffer);

            if (write(receiver_fd, buffer, strlen(buffer)) == -1) {
                perror("Failed to send message to receiver");
                break;
            }
        }
    }

    close(receiver_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
	printf("[S]Server stopped!\n");
    exit(0);
}
