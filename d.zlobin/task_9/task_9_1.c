#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Child process: execute cat\n");
        execlp("cat", "cat", "file.txt", (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        printf("Parent process: text output\n");
    }

    return 0;
}

