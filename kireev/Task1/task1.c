#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <stdio.h>

#define GET_FSLIM 1
#define SET_FSLIM 2

extern char **environ;

int main(int argc, char *argv[])
{
    int c;
    char options[] = "ispuU:cC:dvV:";
    struct rlimit rlp;
    char **p;

    if (argc < 2) {
        fprintf(stderr,"Usage: %s options\n", argv[0]);
        exit(0);
    }

    while ((c = getopt(argc, argv, options)) != -1) {
        switch (c) {
        case 'i':
            printf("RUID: %ld\n", (long)getuid());
            printf("EUID: %ld\n", (long)geteuid());
            printf("RGID: %ld\n", (long)getgid());
            printf("EGID: %ld\n", (long)getegid());
            break;
        case 's':
            setpgid(0, 0);
            break;
        case 'p':
            printf("NPN: %ld\n", (long)getpid());
            printf("PPN: %ld\n", (long)getppid());
            printf("GPN: %ld\n", (long)getpgid(0));
            break;
        case 'U': {
            char *endptr;
            long new_ulimit = strtol(optarg, &endptr, 10);

            if (*endptr != '\0' || new_ulimit < 0) {
                fprintf(stderr, "Invalid ulimit value: %s\n", optarg);
                break;
            }

            if (ulimit(SET_FSLIM, new_ulimit) == -1) {
                fprintf(stderr, "Failed to set ulimit.\n");
            }
            break;
        }
        case 'u':
            printf("Ulimit: %ld\n", ulimit(GET_FSLIM, 0));
            break;
        case 'c':
            getrlimit(RLIMIT_CORE, &rlp);
            printf("Core size: %ld\n", (long)rlp.rlim_cur);
            break;
        case 'C':
            getrlimit(RLIMIT_CORE, &rlp);
            rlp.rlim_cur = atol(optarg);
            if (setrlimit(RLIMIT_CORE, &rlp) == -1)
                fprintf(stderr, "Must be super-user to increase core!\n");
            break;
        case 'd':
            printf("Working directory: %s\n", getcwd(NULL, 100));
            break;
        case 'v':
            printf("Environment variables:\n");
            for (p = environ; *p; p++)
                printf("%s\n", *p);
            break;
        case 'V':
            putenv(optarg);
            break;
        default:
            fprintf(stderr, "Invalid option!\n");
            exit(1);
        }
    }
    return 0;
}