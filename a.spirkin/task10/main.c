#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Pass a command to run");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        execvp(argv[1], argv + 1);
        perror("Error: execvp failed");
        exit(1);
    }
    else if (pid > 0) {
        wait(NULL);
    }
    else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Process ended with the code: %d\n", WEXITSTATUS(status));
        }
        else {
            printf("Process ended abnormally\n");
        }
    }

    return 0;
}

