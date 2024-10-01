#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <sys/param.h>
#include <errno.h>
#include <ulimit.h>

void print_ids() {
    printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
    printf("Real GID: %d, Effective GID: %d\n", getgid(), getegid());
}

void print_pids() {
    printf("PID: %d, PPID: %d, PGID: %d\n", getpid(), getppid(), getpgid(0));
}

void print_ulimit() {
    long ulimit_value = ulimit(UL_GETFSIZE);
    if (ulimit_value == -1) {
        perror("ulimit");
    } else {
        printf("Ulimit: %ld\n", ulimit_value);
    }
}

void print_core_size() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size: %ld bytes\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void change_core_size(long size) {
    struct rlimit rl;
    rl.rlim_cur = size;
    rl.rlim_max = size;
    if (setrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("setrlimit");
    } else {
        printf("Core file size changed to: %ld bytes\n", size);
    }
}

void change_ulimit(long new_limit) {
    if (ulimit(UL_SETFSIZE, new_limit) == -1) {
        perror("ulimit");
    } else {
        printf("Ulimit set to: %ld\n", new_limit);
    }
}

void print_working_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
    }
}

void print_env() {
    extern char **environ;
    char **env = environ;
    while (*env) {
        printf("%s\n", *env);
        env++;
    }
}

void set_env_var(char *name_value) {
    char *equal_sign = strchr(name_value, '=');
    if (equal_sign == NULL) {
        fprintf(stderr, "Invalid format. Use name=value\n");
        return;
    }
    *equal_sign = '\0';
    char *name = name_value;
    char *value = equal_sign + 1;

    if (setenv(name, value, 1) == -1) {
        perror("setenv");
    } else {
        printf("Environment variable %s set to %s\n", name, value);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    long new_ulimit, new_core_size;

    while ((opt = getopt(argc, argv, "ispucC:U:dV:v")) != -1) {
        switch (opt) {
            case 'i':
                print_ids();
                break;
            case 's':
                if (setpgid(0, 0) == -1) {
                    perror("setpgid");
                } else {
                    printf("Process became the leader of the process group.\n");
                }
                break;
            case 'p':
                print_pids();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'c':
                print_core_size();
                break;
            case 'C':
                new_core_size = atol(optarg);
                change_core_size(new_core_size);
                break;
            case 'U':
                new_ulimit = atol(optarg);
                change_ulimit(new_ulimit);
                break;
            case 'd':
                print_working_directory();
                break;
            case 'v':
                print_env();
                break;
            case 'V':
                set_env_var(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-c] [-C size] [-U new_ulimit] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}