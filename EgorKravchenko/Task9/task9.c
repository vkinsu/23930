#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        execlp("cat", "cat", "test.txt", (char *)NULL);
        perror("execlp failed");
        exit(1);
    } else {
        printf("Parent PID: %d\n", getpid());
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(1);
        }
        printf("Parent again\n");
    }
    return 0;
}
