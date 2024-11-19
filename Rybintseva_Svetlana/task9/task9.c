#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        execlp("cat", "cat", "text.txt", NULL);
        perror("execlp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("Родительский процесс: подпроцесс завершен с PID %d\n", pid);
    }

    return 0;
}
