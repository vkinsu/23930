#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    //printf("%ld %ld\n",getpid(),getpgid(getpid()));
    printf("[%ld] parent process id: %ld\n\n",
    getpid(), getppid());
    int code = -10;
    fork();
    if(getpid() != getpgid(getpid())){
        //инструкции для дочернего процесса
        code = execl("/bin/cat","cat","riba.txt",NULL);
        exit(0);
    }
    else{
        //инструкции для родительского процесса
        wait(NULL);
        printf("\n[%ld]dochurka exited\n",getpid());
    }
    printf("[%lld]return exec value %d\n",getpid(),code);
} 