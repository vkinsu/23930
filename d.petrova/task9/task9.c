#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("Failed to create subprocess.\n");
        exit(1);
    }
    if (pid == 0) {
        execlp("cat", "cat", "file.txt", NULL);
        exit(1);
    } 
    else {
        waitpid(pid, &status, 0);
        printf("\nChild finished. Its pid was:  %d\n", pid);
    }
    return 0;
}