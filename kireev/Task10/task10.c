#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(1);
    } else {
		printf("main -> subprocess started...\n");
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(0);
        }

        if (WIFEXITED(status)) {
            printf("main -> process exited with code %d!\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("main -> process killed by signal %d!\n", WTERMSIG(status));
        } else {
            printf("main -> unknow code!\n");
        }
    }

    return 0;
}
