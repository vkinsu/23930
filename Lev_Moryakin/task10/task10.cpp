#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    //printf("%ld %ld\n",getpid(),getpgid(getpid()));
    printf("передаваемые процессу значения:\nкоманда: %s\nаргументы: ",argv[1]);
    for(int i = 1; argv[i]!=NULL;i++)printf("%s ",argv[i]);
    int code = -10;
    fork();
    if(getpid() != getpgid(getpid())){
        //инструкции для дочернего процесса
        printf("\n[%ld] parent process id: %ld\n\n",getpid(), getppid());
        code = execvp(argv[1],&argv[1]);
        exit(0);
    }
    else{
        //инструкции для родительского процесса
        int status;
        wait(&status);
        printf("\n[%ld]dochurka exited\n",getpid());
        if(WIFEXITED(status) != 0){ //успешное завершение
            code = WEXITSTATUS(status);
        }
        else{
            code = -1;
        }
        printf("[%lld]return exec value %d\n",getpid(),code);
    }
    
} 