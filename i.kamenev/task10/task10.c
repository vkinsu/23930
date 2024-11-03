#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Using: %s <comand> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Unable to create subprocess");
        return 1;
    } else if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("Error in execution of the comand");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("Process ended with the code: %d\n", exit_code);
        } else {
            printf("Process ended abnormally\n");
        }
    }

    return 0;
}

