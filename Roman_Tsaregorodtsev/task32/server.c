#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <aio.h>
#include <errno.h>
#include <pthread.h>

#define SOCKET_PATH "./unix_socket"
#define BUFFER_SIZE 1024

void *handle_client(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    struct aiocb aio_read_cb, aio_write_cb;

    while (1)
    {
        memset(&aio_read_cb, 0, sizeof(aio_read_cb));
        aio_read_cb.aio_fildes = client_fd;
        aio_read_cb.aio_buf = buffer;
        aio_read_cb.aio_nbytes = BUFFER_SIZE - 1;
        aio_read_cb.aio_offset = 0;
        aio_read_cb.aio_sigevent.sigev_notify = SIGEV_NONE;

        if (aio_read(&aio_read_cb) == -1)
        {
            perror("aio_read");
            close(client_fd);
            pthread_exit(NULL);
        }

        while (aio_error(&aio_read_cb) == EINPROGRESS)
        {
            usleep(1000);
        }

        int n = aio_return(&aio_read_cb);
        if (n <= 0)
        {
            if (n == 0)
            {
                printf("Client disconnected\n");
            }
            else
            {
                perror("aio_read");
            }
            close(client_fd);
            pthread_exit(NULL);
        }

        buffer[n] = '\0';
        for (int i = 0; i < n; i++)
        {
            buffer[i] = toupper(buffer[i]);
        }

        memset(&aio_write_cb, 0, sizeof(aio_write_cb));
        aio_write_cb.aio_fildes = STDOUT_FILENO;
        aio_write_cb.aio_buf = buffer;
        aio_write_cb.aio_nbytes = n;
        aio_write_cb.aio_offset = 0;
        aio_write_cb.aio_sigevent.sigev_notify = SIGEV_NONE;
        if (buffer[0] == '.')
        {
            sleep(5);
            if (aio_write(&aio_write_cb) == -1)
            {
                perror("aio_write");
                close(client_fd);
                pthread_exit(NULL);
            }
        }
        else
        {
            if (aio_write(&aio_write_cb) == -1)
            {
                perror("aio_write");
                close(client_fd);
                pthread_exit(NULL);
            }
        }

        while (aio_error(&aio_write_cb) == EINPROGRESS)
        {
            usleep(1000);
        }

        if (aio_return(&aio_write_cb) == -1)
        {
            perror("aio_write");
            close(client_fd);
            pthread_exit(NULL);
        }
    }
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    pthread_t thread_id;

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;
        if (pthread_create(&thread_id, NULL, handle_client, client_fd_ptr) != 0)
        {
            perror("pthread_create");
            close(client_fd);
            free(client_fd_ptr);
            continue;
        }
        pthread_detach(thread_id);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
