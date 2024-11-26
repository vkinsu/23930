#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./upper_case_socket"

int main() {
    int sock_fd;
    struct sockaddr_un server_addr;
    char message[1024];

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Enter a string to send to the server: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0'; 

    if (write(sock_fd, message, strlen(message)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(sock_fd);

    return 0;
}

