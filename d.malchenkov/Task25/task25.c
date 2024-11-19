#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    int pipefd[2]; // Дескрипторы канала: pipefd[0] для чтения, pipefd[1] для записи
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
        ssize_t bytes_read;

        while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            for (ssize_t i = 0; i < bytes_read; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }
            write(STDOUT_FILENO, buffer, bytes_read); 
        }

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else { 
        // Родительский процесс
        close(pipefd[0]); 

        printf("Введите текст:\n");
        ssize_t bytes_read;
        while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
            write(pipefd[1], buffer, bytes_read); 
        }

        close(pipefd[1]); 
        wait(NULL); 
        exit(EXIT_SUCCESS);
    }
}
