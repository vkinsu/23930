#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() 
{
    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0) 
    {
        execlp("cat", "cat", "file.txt", (char *)NULL);
        perror("execlp failed");
        exit(1);
    } 
    else
    {
        printf("Parent: I'm printing this while the child is running\n");

        waitpid(pid, NULL, 0);

        printf("Parent: The child has finished, now I can print this\n");
    }

    return 0;
}