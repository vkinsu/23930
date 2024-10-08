#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <limits.h>


extern char **environ;

int main(int argc, char *argv[]){
    int command;
    struct rlimit rl;
    char cwd[_PC_PATH_MAX];

    while ((command = getopt(argc, argv, "ispuU::cC:dV::v")) != EOF){
        switch (command)
        {
        case 'i':
            printf("uid=%d, euid=%d, gid=%d, egid=%d\n", getuid(), geteuid(), getgid(), getegid());
            break;
        
        case 's':
            if (setpgid(0, 0) == 0){
            }
            else{
                perror("error: Could not set group leader process\n");
            }
            break;
        
        case 'p':
            printf("pid=%d, ppid=%d, pgid=%d\n", getpid(), getppid(), getpgid(0));
            break;
        
        case 'u':
            if (getrlimit(RLIMIT_FSIZE, &rl) == 0) {
                printf("Ulimit value: %lu\n", rl.rlim_cur);
            }
            else{
                perror("Failed to get ulimit value\n");
            }
            break;

        case 'U':
            unsigned long ulimit_renewed = strtol(optarg, NULL, 10);
            
            if (getrlimit(RLIMIT_FSIZE, &rl) != 0) {
                perror("Failed to set ulimit value\n");
                break;
            }

            rl.rlim_cur = ulimit_renewed;
            if (-1 == setrlimit(RLIMIT_FSIZE, &rl)) {
            perror("Failed to set ulimit value\n");
            }
            break;

        case 'c':
            if (getrlimit(RLIMIT_CORE, &rl) == 0) {
                printf("Core file max size: %lu\n", rl.rlim_max);
            }
            else {
                perror("error: Faild to get core file max size\n");
            }
            break;

        case 'C':
            break;

        case 'd':
            printf("Current working directory is: %s\n", getcwd(NULL,100));
            break;

        case 'V':
            if (putenv(optarg) != 0) {
                perror("Failed to set new environmental variable\n");
            }

        case 'v':
            char **pointer = environ;
            while (*pointer != NULL) {
                printf("%s\n", *pointer);
                pointer++;
            }
            break;
        }
    }
    return 0;
    
}