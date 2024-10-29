#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(0);
    }
	
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        execlp("cat", "cat", argv[1], (char *)NULL);
        perror("execlp failed");
        exit(1);
    } else {
        printf("main -> subprocess started...\n");

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(0);
        }
		
        printf("main -> subprocess finished!\n");
    }

    exit(0);
}
