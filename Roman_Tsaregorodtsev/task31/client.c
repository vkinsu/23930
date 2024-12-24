#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_socket"

int main()
{
    int client_sock;
    struct sockaddr_un server_addr;
    char buffer[1024];
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
       // printf("Enter text to send: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (send(client_sock, buffer, strlen(buffer), 0) < 0)
        {
            perror("send");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
    }
    close(client_sock);

    return 0;
}
