#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./upper_case_socket"

void to_upper_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[1024];

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
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s...\n", SOCKET_PATH);


   if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    ssize_t num_bytes;
    while ((num_bytes = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[num_bytes] = '\0';

        to_upper_case(buffer);

        printf("Received and converted: %s\n", buffer);
    }

    if (num_bytes == -1) {
        perror("read");
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}

