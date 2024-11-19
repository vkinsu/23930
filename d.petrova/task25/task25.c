#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) == -1) {
        perror("Failed to create pipe.");
        exit(1);
    }
    pid = fork();
    if (pid < 0) {
        perror("Failed to create fork.");
        exit(1);
    }

    if (pid == 0) {
        close(pipe_fd[1]);

        char buffer[2048];
        int bytesRead;
        while ((bytesRead = read(pipe_fd[0], buffer, 2048)) > 0) {
            for (ssize_t i = 0; i < bytesRead; i++) 
                buffer[i] = toupper(buffer[i]);
            
            write(STDOUT_FILENO, buffer, bytesRead);
        }

        if (bytesRead == -1) {
            perror("Failed to read from pipe.");
            exit(1);
        }
        close(pipe_fd[0]);
        exit(0);

    } 
    else {
        close(pipe_fd[0]);

        const char *text = "Test.\nHello world\n";
        write(pipe_fd[1], text, strlen(text));

        close(pipe_fd[1]);
        wait(NULL);
    }

    return 0;
}
