#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#define SOCKET_PATH "./socket"
#define MAX_CLIENTS 2
#define answer1 "str2\0"
#define answer2 "str4\0"

int active_clients = 0;
int client_fds[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int running = 1;
int massages = 0;

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    char buffer[1024];
    ssize_t bytes_received;
    int client_id = -1;
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client_fds[i] == -1) {
            client_fds[i] = client_fd;
            client_id = i;
            active_clients++;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    if (client_id == -1) {
        printf("Server can only handle %d clients\n", MAX_CLIENTS);
        close(client_fd);
        free(arg);
        return NULL;
    }
    while ((bytes_received = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
        pthread_mutex_lock(&clients_mutex);
        int recipient_id = (client_id == 0) ? 1 : 0;
        if (client_fds[recipient_id] != -1) {
            if (recipient_id == 0) {
                if (write(client_fds[recipient_id], answer2, sizeof(answer2)) == -1) {
                    perror("Error sending message to client");
                }
            } else {
                if (write(client_fds[recipient_id], answer1, sizeof(answer1)) == -1) {
                    perror("Error sending message to client");
                }
            }
        }
        massages++;
        if (massages >= 2000) {
          running = 0;
          close(client_fd);
          pthread_mutex_lock(&clients_mutex);
          client_fds[client_id] = -1;
          active_clients--;
          pthread_mutex_unlock(&clients_mutex);
          break;
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    close(client_fd);
    pthread_mutex_lock(&clients_mutex);
    client_fds[client_id] = -1;
    active_clients--;
    pthread_mutex_unlock(&clients_mutex);
    printf("Client %d disconnected\n", client_id);
    free(arg);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un address;
    struct sockaddr_un client_address;
    socklen_t client_len;
    pthread_t client_thread;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_fds[i] = -1;
    }
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }
    unlink(SOCKET_PATH);
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Server connection error");
        close(server_fd);
        exit(1);
    }
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Socket listening error");
        close(server_fd);
        exit(1);
    }
    while (running) {
        client_len = sizeof(client_address);
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
        if (client_fd == -1) {
            perror("Accept error");
            continue;
        }
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;
        if (pthread_create(&client_thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("Thread creation error");
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

