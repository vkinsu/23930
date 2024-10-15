#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

void print_ids() {
    printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
    printf("Real GID: %d, Effective GID: %d\n", getgid(), getegid());
}

void become_group_leader() {
    if (setpgid(0, 0) == -1) {
        perror("setpgid");
    } else {
        printf("Process became a group leader\n");
    }
}

void print_process_ids() {
    printf("Process ID: %d, Parent Process ID: %d, Process Group ID: %d\n", getpid(), getppid(), getpgrp());
}

void print_ulimit() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("File size limit: %ld\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void set_ulimit(const char *new_ulimit) {
    struct rlimit rl;
    rl.rlim_cur = atol(new_ulimit);
    rl.rlim_max = atol(new_ulimit);
    if (setrlimit(RLIMIT_FSIZE, &rl) == -1) {
        perror("setrlimit");
    } else {
        printf("Limit set to: %ld\n", rl.rlim_cur);
    }
}

void print_core_size() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size limit: %ld\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void set_core_size(const char *size) {
    struct rlimit rl;
    rl.rlim_cur = atol(size);
    rl.rlim_max = atol(size);
    if (setrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("setrlimit");
    } else {
        printf("Limit set to: %ld\n", rl.rlim_cur);
    }
}

void print_current_directory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
    }
}

void print_environment_variables() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        printf("%s\n", *env);
    }
}

void set_environment_variable(const char *name_value) {
    char *name = strdup(name_value);
    char *value = strchr(name, '=');
    if (value != NULL) {
        *value++ = 0;
        if (setenv(name, value, 1) == 0) {
            printf("Environment variable %s set to %s\n", name, value);
        } else {
            perror("setenv");
        }
    } else {
        fprintf(stderr, "Invalid format: %s\n", name_value);
    }
    free(name);
}

int main(int argc, char *argv[]) {
    int option;
    while ((option = getopt(argc, argv, "ispuU:cC:dvV:")) != -1) {
        switch (option) {
            case 'i':
                print_ids();
                break;
            case 's':
                become_group_leader();
                break;
            case 'p':
                print_process_ids();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'U':
                set_ulimit(optarg);
                break;
            case 'c':
                print_core_size();
                break;
            case 'C':
                set_core_size(optarg);
                break;
            case 'd':
                print_current_directory();
                break;
            case 'v':
                print_environment_variables();
                break;
            case 'V':
                set_environment_variable(optarg);
                break;
            default:
                fprintf(stderr, "Invalid input");
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}
