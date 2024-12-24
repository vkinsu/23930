#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./unix_socket"

int main()
{
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[1024];
    int bytes_received;
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH);
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    if (listen(server_sock, 1) == -1)
    {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    client_len = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0)
    {
        perror("accept");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
    if (bytes_received < 0)
    {
        perror("recv");
        close(client_sock);
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < bytes_received; i++)
    {
        buffer[i] = toupper(buffer[i]);
        printf("%c", buffer[i]);
    }
    printf("\n");
    close(client_sock);
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
