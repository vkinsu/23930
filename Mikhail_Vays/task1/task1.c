#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#include <limits.h>
#include <ulimit.h>
#include <sys/time.h>

void print_ids() {
    printf("Real user ID: %d | Effective user ID: %d\n", getuid(), geteuid());
    printf("Real group ID: %d | Effective group ID: %d\n", getgid(), getegid());
}

void set_process_new_group_leader() {
    if (setpgid(0, 0) == -1) {
        perror("Failed to set process group leader");
    } else {
        printf("Process is now the leader of its group\n");
    }
}

void print_process_info() {
    printf("PID: %d\n", getpid());
    printf("Parent PID: %d\n", getppid());
    printf("PGID: %d\n", getpgid(0));
}

void print_ulimit() {
    printf("ulimit: %ld\n", ulimit(1, 0));
}

void set_ulimit(const char *new_limit) {
    long new_size = atol(new_limit);
    if (ulimit(UL_SETFSIZE, new_size) == -1) {
        perror("Error setting ulimit");
    } else {
        printf("New ulimit set to: %ld\n", new_size);
    }
}

void print_core_limit() {
    struct rlimit core_limit;
    if (getrlimit(RLIMIT_CORE, &core_limit) == -1) {
        perror("error getting core limit");
    }
    else {
        printf("current core file size limit: %ld\n", core_limit.rlim_cur);
    }
}

void set_core_limit(const char *size_str) {
    long new_size = atol(size_str);
    struct rlimit core_limit;
    core_limit.rlim_cur = new_size;
    core_limit.rlim_max = new_size;
    if (setrlimit(RLIMIT_CORE, &core_limit) == -1) {
        perror("Error setting core limit");
    } else {
        printf("New core file size limit set to: %ld\n", new_size);
    }
}

void get_working_directory() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    }
    else {
        perror("Error getting current working directory\n");
    }
}

void print_env_vars() {
    extern char** environ;
    printf("Env vars:\n");
    for (char** p = environ; *p; p++) {
        printf("%s\n", *p);
    }
}

int main(int argc, char *argv[]) {
    char options[] = "ispucC:U:dvV:";
    int opt;

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            case 'i': 
                print_ids();
                break;
            case 's': 
                set_process_new_group_leader();
                break;
            case 'p': 
                print_process_info();
                break;
            case 'u': 
                print_ulimit();
                break;
            case 'U': 
                set_ulimit(optarg);
                break;
            case 'c': 
                print_core_limit();
                break;
            case 'C': 
                set_core_limit(optarg);
                break;
            case 'd': 
                get_working_directory();
                break;
            case 'v': 
                print_env_vars();
                break;
            case 'V':
                putenv(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}