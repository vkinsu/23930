    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <linux/limits.h>
    #include <string.h>
    #include <sys/resource.h>

    extern char *optarg;  
    extern int optind;    
    extern int optopt;    

    int main(int argc, char *argv[]) {
        char options[] = "ispuU:cC:dvV:";  /* valid options */
        int c, invalid = 0, dflg = 0, fflg = 0, gflg = 0;
        struct rlimit process;
        char *f_ptr = NULL, *g_ptr = NULL;

        printf("argc equals %d\n", argc);
        while ((c = getopt(argc, argv, options)) != -1) { 
            switch (c) {
            case 'i':
                printf("Real UID: %d\n", getuid());
                printf("Effective UID: %d\n", geteuid());
                printf("Real GID: %d\n", getgid());
                printf("Effective GID: %d\n", getegid());
                break;
            case 'd':
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) 
                {
                    printf("Current working directory: %s\n", cwd);
                }
                else 
                {
                    perror("getcwd");
                }
                dflg++;
                break;

            case 's':
                if (setpgid(0, 0) == -1) {
                    exit(EXIT_FAILURE);
                }
                printf("Process group ID successfully set.\n");
                break;

            case 'u':
                struct rlimit r;
                if (getrlimit(RLIMIT_FSIZE, &r) == 0) {
                    printf("Current file size limit: %lu bytes\n", r.rlim_cur);
                }
                else {
                    perror("Failed to get file size limit");
                }
                break;

            case 'U':
                long new_limit = atol(optarg);
                struct rlimit rl;
                if (getrlimit(RLIMIT_FSIZE, &rl) == -1) {
                    perror("getrlimit");
                } 
                else {
                    // Устанавливаем новый лимит
                    rl.rlim_cur = new_limit;
                    if (setrlimit(RLIMIT_FSIZE, &rl) == -1) {
                        perror("setrlimit");
                    } 
                    else {
                        printf("File size limit set to: %ld bytes\n", new_limit);
                    }
                }
                break;

            case 'c':
                if (getrlimit(RLIMIT_CORE, &process) == 0) {
                    printf("Byte size of core-file: %lu\n", process.rlim_cur);
                } 
                else {
                    perror("Failed to get core-file size\n");
                }
                break;
                
            case 'C':
                unsigned long core_size_renewed = strtol(optarg, NULL, 10);
                if (core_size_renewed <= 0) {
                    perror("Failed to set core size\n");
                    break;
                }
                process.rlim_cur = core_size_renewed;
                process.rlim_max = core_size_renewed;
                if (setrlimit(RLIMIT_CORE, &process) == -1) {
                    perror("Failed to set core file size limit");
                } 
                else {
                    printf("Core file size limit set to: %lu bytes\n", core_size_renewed);
                }
                break; 

            case 'p':
                printf("PID: %d\n", getpid());
                printf("PPID: %d\n", getppid());
                printf("PGID: %d\n", getpgrp());
                break;

            case 'v':
                extern char **environ;
                char **env = environ;
                while (*env) {
                    printf("%s\n", *env++);
                }
                break;

            case 'V':
                if (putenv(optarg) == 0) {
                    printf("New environmental variable is set\n");
                } else {
                    perror("Failed to set new environmental variable\n");
                }
            }
        }

        return 0;
    }
