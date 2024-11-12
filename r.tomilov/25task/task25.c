#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);

        ssize_t bytesRead = read(pipefd[0], buffer, BUFFER_SIZE);
        if (bytesRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        int i;
        for (i = 0; i < bytesRead; i++) {
          buffer[i] = toupper(buffer[i]);
        }

        printf("Преобразованный текст: %s\n", buffer);

        close(pipefd[0]);
    } else {
        close(pipefd[0]);

        printf("Введите текст: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        ssize_t bytesWritten = write(pipefd[1], buffer, strlen(buffer) + 1);
        if (bytesWritten == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]);
    }

    return 0;
}
