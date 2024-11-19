#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void to_upper_and_print(int read_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(read_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';

        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }
        
        printf("Transformed text: %s", buffer);
    }

    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
}

void write_text(int write_fd) {
    const char *text = "Test example: Hello, World!\n123abcABC\n";
    size_t len = strlen(text);

    if (write(write_fd, text, len) != len) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(write_fd);
}

int main() {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {

        close(pipe_fd[1]);
        to_upper_and_print(pipe_fd[0]);
        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    } else {
        
        close(pipe_fd[0]);
        write_text(pipe_fd[1]);
        close(pipe_fd[1]);

        wait(NULL);
    }

    return EXIT_SUCCESS;
}

