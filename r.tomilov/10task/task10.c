#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) 
    {
        execvp(argv[1], &argv[1]);

        perror("execvp failed");
        exit(1);
    } else 
    {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) 
        {
            printf("Process exited with code %d\n", WEXITSTATUS(status));
        } 
        else 
        {
            printf("Process terminated abnormally\n");
        }
    }

    return 0;
}