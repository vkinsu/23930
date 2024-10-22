#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;

    if (argc < 2) {
        fprintf(stderr, "Not enough arguments.\n");
        exit(1);
    }
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Failed to create subprocess.\n");
        exit(1);
    }
    else if (pid == 0)
        execvp(argv[1], &argv[1]);
    else {
        if (waitpid(pid, &status, 0) == -1) {
            fprintf(stderr, "waitpid failed");
            exit(1);
        }
        if (WIFEXITED(status)) 
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status)) 
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        
    }
    return 0;
}