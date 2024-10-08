#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

extern char **environ;

int main(int argc, char *argv[]) {
    int c;
    char options[] = "ispvuU:cC:V:d";
    struct rlimit rlp;
    char **p;
    while ((c = getopt(argc, argv, options)) != -1) {
        switch(c){
            case 'i':
                printf("RUID = %d\n", getuid());
                printf("EUID = %d\n", geteuid());
                printf("RGID = %d\n", getgid());
                printf("EGID = %d\n", getegid());
                break;
            case 's':
                if (setpgid(0, 0) == -1) perror("Fail s");
                break;
            case 'p':
                printf("PID = %ld\n", (long)getpid());
                printf("PPID = %ld\n", (long)getppid());
                printf("GPID = %ld\n", (long)getpgid(0));
                break;
            case 'u':
                printf("Ulimit = %ld\n", ulimit(UL_GETFSIZE));
                break;
            case 'U': {
                long new_limit = atol(optarg);
                if (ulimit(UL_SETFSIZE, new_limit) == -1) perror("Fail U");
                break;
            }
            case 'c':
                if (getrlimit(RLIMIT_CORE, &rlp) != 0) perror("Fail c");
                else printf("RLIMIT_CORE = %ld\n", (long)rlp.rlim_cur);
                break;
            case 'C': {
                long new_core_size = atol(optarg);
                if (new_core_size < 0) {
                    fprintf(stderr, "Invalid value\n");
                    break;
                }
                if (getrlimit(RLIMIT_CORE, &rlp) == 0) {
                    rlp.rlim_cur = new_core_size;
                    if (setrlimit(RLIMIT_CORE, &rlp) == -1) perror("Fail C");
                } else perror("Fail C");
                break;
            }
            case 'd': {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) printf("Curr dir: %s\n", cwd);
                else perror("Fail d");
                break;
            }
            case 'v':
                printf("Environ:\n");
                for (p = environ; *p != NULL; p++) printf("%s\n", *p);
                break;
            case 'V': {
                if (putenv(optarg) != 0) perror("Fail V");
                break;
            }
            default:
                fprintf(stderr, "Unknown option\n");
                break;
        }
    }
    return 0;
}

