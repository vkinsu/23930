#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_socket_example"
#define BUFFER_SIZE 1024

typedef struct {
    int fd;
    int word_count; 
    char word_buffer[2][BUFFER_SIZE];
} ClientState;

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    fd_set read_fds, temp_fds;
    int max_fd;
    ClientState client_states[FD_SETSIZE] = {0};
    int total_word_count = 0;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Failed to bind.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Failed to listen.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    printf("Server started...\n");

    while (1) {
        temp_fds = read_fds;

        if (select(max_fd + 1, &temp_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &temp_fds)) {
                if (i == server_fd) {
                    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                        perror("Failed to accept connection");
                    }
                    else {
                        FD_SET(client_fd, &read_fds);
                        if (client_fd > max_fd) max_fd = client_fd;
                        client_states[client_fd].fd = client_fd;
                        client_states[client_fd].word_count = 0;
                        printf("New client connected: FD %d\n", client_fd);
                    }
                } 
                else {
                    char buffer[BUFFER_SIZE];
                    ssize_t bytes_read = read(i, buffer, BUFFER_SIZE - 1);
                    if (bytes_read <= 0) {
                        if (bytes_read == 0) {
                            printf("Client disconnected: FD %d\n", i);
                        } 
                        else {
                            perror("Failed to read");
                        }
                        close(i);
                        FD_CLR(i, &read_fds);
                        memset(&client_states[i], 0, sizeof(ClientState));
                    }
                    else {
                        buffer[bytes_read] = '\0';
                        ClientState *state = &client_states[i];

                        if (strncmp(buffer, "client", 6) == 0 || strncmp(buffer, "isright", 7) == 0) {
                            strncpy(state->word_buffer[state->word_count], buffer, BUFFER_SIZE - 1);
                            state->word_count++;

                            if (state->word_count == 2) {
                                char response[BUFFER_SIZE];
                                total_word_count++;
                                snprintf(response, BUFFER_SIZE, "%s %s%d\n",
                                         state->word_buffer[0],
                                         state->word_buffer[1],
                                         total_word_count);

                                write(i, response, strlen(response));
                                printf("Sent to client %d: %s\n", i, response);

                                state->word_count = 0;
                            }
                        } 
                        else {
                            printf("Received from client %d: %s\n", i, buffer);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
