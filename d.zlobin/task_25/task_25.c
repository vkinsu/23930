#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe_fds[2];
    pid_t pid;
    
    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fds[1]);

        char buffer[BUFFER_SIZE];
        int n;

        while ((n = read(pipe_fds[0], buffer, sizeof(buffer))) > 0) {
            for (int i = 0; i < n; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }
            write(STDOUT_FILENO, buffer, n);
        }

        close(pipe_fds[0]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipe_fds[0]);

        const char *text = "New text output with Mixed litterals CASE.\n";
        
        write(pipe_fds[1], text, strlen(text));

        close(pipe_fds[1]);
        wait(NULL);

        exit(EXIT_SUCCESS);
    }

    return 0;
}
