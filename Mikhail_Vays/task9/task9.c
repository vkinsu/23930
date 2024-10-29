#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        printf("Подпроцесс (PID=%d) запускает команду cat\n", getpid());
        
        execlp("cat", "cat", "longfile.txt", (char *)NULL);
        
        perror("execlp failed");
        exit(1);
    } else {
        printf("Родительский процесс (PID=%d): Запущен подпроцесс PID=%d\n", getpid(), pid);
        
        if (waitpid(pid, &status, 0) == -1) {
            perror("failed executing waitpid()");
            exit(0);
        }
        
        printf("Родительский процесс: Подпроцесс PID=%d завершился с кодом %d\n", pid, WEXITSTATUS(status));
    }

    return 0;
}
