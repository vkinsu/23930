#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); 

    if (pid < 0) {
        perror("fork failed");
        return 1; 
    } else if (pid == 0) {
        printf("Child is running\n");
        execlp("cat", "cat", "file.txt", NULL);
        perror("execlp failed"); 
        exit(1);
    } else {   
        wait(NULL);
        printf("Parent: Child proccess is finished");
        printf("\n");
    }

    return 0;
}
