#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    if (pipe(pipefd) == -1) {
        perror("can't create pipe");
        exit(1);
    }

   	pid = fork();
    if (pid == -1) {
        perror("error in fork");
        exit(1);
    }

    if (pid == 0) {
        close(pipefd[1]);
        int n;
        while ((n = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            for (int i = 0; i < n; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            write(STDOUT_FILENO, buffer, n);
        }
        close(pipefd[0]);
        exit(0);
    } else {
        close(pipefd[0]);
        printf("Enter text (Control-D - exit):\n");
        while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
            write(pipefd[1], buffer, strlen(buffer));
        }
        close(pipefd[1]);
        wait(NULL);
    }
    return 0;
}

