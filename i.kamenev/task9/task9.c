#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Unable to create subprocess");
        return 1;
    } else if (pid == 0) {
        execlp("cat", "cat", "task5.c", (char *)NULL);
        perror("Error in execution cat");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        
        printf("Subprocess ended. This is text form parent process.\n");
    }

    return 0;
}

