
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <ulimit.h>
#include <sys/resource.h>
//extern char **environment;
int main(int argc,char **argv,char **envp){
    setlocale(LC_ALL,"rus");
    rlimit rlim;
    char buf[100];
    int size = 100;
    int opt;
    char **endptr;
    long int ul;
    printf("\n--------------------------\n");
    while(opt = getopt(argc,argv,"ispdvucV:U:C:")){
        if(opt == -1)break;
        switch (opt){
            case 'i': 
                printf("Пользователь\n");
                printf("Реальный id: %d\n",getuid());
                printf("Эффективный id: %d\n",geteuid());

                printf("Группа\n");
                printf("Реальный id: %d\n",getgid());
                printf("Эффективный id: %d\n",getegid());
                break;
            case 's':
                if(setpgid(0,getpgid(0)) == -1)printf("\nerror changing process id\n");
                else  printf("\nchanged process id\n");
                break;
            case 'p': 
                printf("\nИдентификатор процесса: %d\n",getpid());
                printf("Идентификатор родительского процесса: %d\n",getppid());
                printf("Идентификатор группы процесса: %d\n",getpgid(0));
                break;
            case 'u': 
                ul = ulimit(UL_GETFSIZE,0);
                if(ul == 0)printf("ulimit size check failure");
                else printf("ulimit = %lld",ul);
                break;
            case 'd':
                getcwd(buf, size);
                printf("\ncurrent directory:\n %s\n",buf);
                break;
            case 'v':   
                for(int i = 0; envp[i] != NULL; i++){
                    printf("%s\n",envp[i]);
                }
                break;
            case 'c':
                getrlimit(RLIMIT_CORE,&rlim);
                printf("%d %d %d = inf",rlim.rlim_cur,rlim.rlim_max,RLIM_INFINITY);
                break;
            case 'V':
                if(putenv(optarg) == 0)printf("putenv success");
                else printf("putenv error");
                break;
            case 'U':
                ul = atoi(optarg);
                if(ul == 0)printf("wrong value format");
                else printf("set ulimit to %ld",ulimit(UL_SETFSIZE,ul));
                break;
            case 'C':
                rlim.rlim_cur = atoi(optarg);
                rlim.rlim_max = -1;
                if(setrlimit(RLIMIT_CORE,&rlim) == -1)printf("error changing core file size");
                else printf("changed core file size to %d",rlim.rlim_cur);
                break;
            case '?': printf("\nnot found\n"); break;
        }
        printf("\n--------------------------\n");

    }

}