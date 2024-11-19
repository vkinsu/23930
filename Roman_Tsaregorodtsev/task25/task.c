#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int pipefd[2];
    pid_t cpid;
    char buf;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    int status;
    if (cpid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0)
    {
        close(pipefd[0]);
        const char *text = "Hello, World! This is a test.\n";
        write(pipefd[1], text, strlen(text));
        close(pipefd[1]);
        _exit(EXIT_SUCCESS);
    }
    else
    {
        close(pipefd[1]);
        while (read(pipefd[0], &buf, 1) > 0)
        {
            buf = toupper(buf);
            write(STDOUT_FILENO, &buf, 1);
        }
        close(pipefd[0]);
        waitpid(cpid, &status, 0);
        exit(EXIT_SUCCESS);
    }
}
