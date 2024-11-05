#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(1);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(1);
        }
        if (WIFEXITED(status)) {
            printf("Child PID: %d, finish code %d\n",
                    pid, WEXITSTATUS(status));
        }
    }
    return 0;
}

