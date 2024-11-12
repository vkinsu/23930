#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        printf("Child process has started execution.\n");

        execvp(argv[1], &argv[1]);
        perror("execvp");
        return 1;
    } else {
        printf("Parent process is running.\n");

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        printf("Child process has finished.\n");

        if (WIFEXITED(status)) {
            printf("Exit code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Terminated by signal: %d\n", WTERMSIG(status));
        } else {
            printf("Process did not exit normally\n");
        }

        printf("Parent process has finished.\n");
    }

    return 0;
}
