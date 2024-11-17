#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	if (argc != 2) {
        printf("Usage: %s <message>\n", argv[0]);
        exit(0);
    }
	
    int pipe_fd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fd[1]);

        ssize_t bytes_read;
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';

            for (int i = 0; buffer[i] != '\0'; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }

            printf("Child process received: %s\n", buffer);
        }

        if (bytes_read == -1) {
            perror("read");
        }

        close(pipe_fd[0]);
    } else {
        close(pipe_fd[0]);

        const char *message = argv[1];
        printf("Parent process sending: %s\n", message);
        if (write(pipe_fd[1], message, strlen(message)) == -1) {
            perror("write");
        }

        close(pipe_fd[1]);

        wait(NULL);
    }

    return 0;
}
