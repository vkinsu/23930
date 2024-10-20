#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("File name required. And nothing more.\n");
        return 1;
    }

    char *f = argv[1];

    pid_t pid = fork();

    if (pid == 0) {
        execlp("cat", "cat", f, NULL);
        perror("Error on handling execlp during a subprocess.");
        return 1;
    }
    else if (pid > 0) {
        printf("Parent process: \"gotta sleep for a while\".\n");
        sleep(3);
        printf("Parent process: \"good morning yall\"\n");

        wait(NULL);

        printf("Subprocess has ended.\n");
    }
    else {
        perror("Error on creating a subprocess (fork() error).");
        return 1;
    }

    return 0;
}

