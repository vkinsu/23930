#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe_fd[2];
    pid_t child_pid;

    if (pipe(pipe_fd) == -1) {
        perror("pipe command error");
        exit(1);
    }

    if ((child_pid = fork()) == -1) {
        perror("fork command error");
        exit(1);
    }

    if (child_pid == 0) {
                            // Это дочерний процесс (пишет в канал)
        close(pipe_fd[0]);  // Закрываем конец канала для чтения

        const char *text = "Hello World! This is a test of the PIPE.\n";
        write(pipe_fd[1], text, strlen(text));
        close(pipe_fd[1]);  // Закрываем конец канала после записи

        exit(0);
    } else {
                            // Это родительский процесс (читает из канала и преобразует текст)
        close(pipe_fd[1]);  // Закрываем конец канала для записи

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';

            for (int i = 0; i < bytes_read; i++) {
                buffer[i] = toupper((unsigned char) buffer[i]);
            }

            printf("Transformed text: \n%s", buffer);
        }

        close(pipe_fd[0]);  // Закрываем конец канала после чтения

        wait(NULL); // Ожидаем завершения дочернего процесса
    }

    return 0;
}
