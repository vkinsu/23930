#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc < 2){
        printf("too few arguments");
        exit(EXIT_FAILURE);
    }
    pid_t p_id = fork();
    int status;

    printf("%d\n", p_id);

    if (p_id < 0) {
        printf("fork failed");
        exit(1);
    }

    if (p_id == 0) {
        execvp(argv[1], &argv[1]);
        printf("execlp failed");
        exit(EXIT_FAILURE);
    }
    
    else {
        waitpid(p_id, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child process: subprocess terminated with PID %d\n", p_id, WEXITSTATUS(status));
        }
    }
    

    return 0;
}
