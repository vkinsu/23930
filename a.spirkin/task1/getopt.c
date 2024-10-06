#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <ulimit.h>
#include <getopt.h>
#include <sys/resource.h>
#include <sys/types.h>

#define ENV_LENGTH 16

extern char **environ;

int main(int argc, char *argv[])
{
    char options[ ] = "ispuU:cC:dvV:";
    int c, invalid = 0;
    long int ulim;
    struct rlimit rlim;

    char cwd[PATH_MAX];
    char *envs;

    while ((c = getopt(argc, argv, options)) != -1) {
        switch (c) {
            case 'd':               
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("-d: CWD: %s\n", cwd);
                }
                else {
                    perror("-d: Error on getting current working directory\n");                
                }
                break;
            case 'v':
                printf("-v: Getting environment variables:\n");
                for (char **env = environ; *env; ++env) {
                    printf("%s\n", *env);
                }
                break;
            case 'i':
                printf("-i: Getting user identifiers:\n");
                printf("\tUID: %d\n", getuid());
                printf("\tGID: %d\n", getgid());
                printf("\tEUID: %d\n", geteuid());
                printf("\tEGID: %d\n", getegid());
                break;
            case 'p':
                printf("-p: Getting process identifiers:\n");
                printf("\tCurrent process ID: %d\n", getpid());
                printf("\tParent process ID: %d\n", getppid());
                printf("\tProcess group ID: %d\n", getpgid(0));
		break;
            case 's':
                if (setpgid(0, getpgid(0)) == -1) {
                    printf("-s: Error on handling process change\n");
                }
                else {
                    printf("-s: Changed process\n");
                }
                break;
            case 'u': 
                ulim = ulimit(UL_GETFSIZE, 0);
                if (ulim == 0) {
                    printf("-u: Error on getting ulimit size\n");
                }
                else {
                    printf("-u: Ulimit equals: %ld\n", ulim);
                }
                break;
            case 'c':
                getrlimit(RLIMIT_CORE, &rlim);
                printf("-c: Getting core file limits:\n");
                printf("\tcore files soft limit: %4ld\n", rlim.rlim_cur);
                printf("\tcore files hard limit: %4ld\n", rlim.rlim_max);
                break;
            case 'V':
                if (putenv(optarg) == 0) {
                    printf("-V: Added environment variable: %s\n", optarg);
                }
                else {
                    printf("-V: Error on handling putenv()\n");
                }
                break;

            // The code below was worth trying so it's here.
                // char* env;
                // char* value;

                // env = (char*)malloc(ENV_LENGTH * sizeof(char));

                // for (int i = 0; i < ENV_LENGTH; i++) {
                //     if (optarg[i] != '=') {
                //         env[i] = optarg[i];
                //         value = &optarg[i];
                //     }
                //     else break;
                // }
                // value += 2;
                // printf("%s", env);
                // printf("%s", value);

                // if (env && value) {
                //     if (setenv(env, value, 1) == 0) {
                //         printf("-V: Added environment variable: %s=%s\n", env, value);
                //     }
                //     else {
                //         printf("-V: Error on handling setenv().\n");
                //     }
                // }
                // else {
                //     printf("-V: Invalid environment variable format.\n");
                // }

                // free(env);
                break;
            case 'C':
                getrlimit(RLIMIT_CORE, &rlim);
                rlim.rlim_cur = atoi(optarg);
                rlim.rlim_max = -1;
                if (setrlimit(RLIMIT_CORE, &rlim) == -1) {
                    printf("-C: Error on handling core file size change");
                }
                else {
                    printf("-C: Core file size changed to %ld", rlim.rlim_cur);
                }
                break;
            case 'U':
                ulim = atoi(optarg);
                if (ulim == 0) {
                    printf("-U: Wrong value format");
                }
                else {
                    printf("-U: Set ulimit to %ld", ulimit(UL_SETFSIZE, ulim));
                }
                break;
            case '?':
                printf("Invalid option is %c\n", optopt);
                invalid++;
        }
    }

    printf("\n");
    printf("optind amount: %d\n", optind);
    printf("invalid inputs amount: %d\n", invalid);
    return 0;
}
