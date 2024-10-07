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

void print_user_info() {
    uid_t real_uid = getuid();
    uid_t eff_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t eff_gid = getegid();

    printf("Real UID: %d, Effective UID: %d\n", real_uid, eff_uid);
    printf("Real GID: %d, Effective GID: %d\n", real_gid, eff_gid);
}

void set_process_group_leader() {
    if (setpgid(0, 0) == -1) {
        perror("Failed to set process group leader");
    } else {
        printf("Process is now the leader of its group\n");
    }
}

void print_process_info() {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgid(0);

    printf("Process ID (PID): %d\n", pid);
    printf("Parent Process ID (PPID): %d\n", ppid);
    printf("Process Group ID (PGID): %d\n", pgid);
}

void print_ulimit() {
    struct rlimit limit;
    getrlimit(RLIMIT_FSIZE, &limit);

    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("Current ulimit: unlimited\n");
    } else {
        printf("Current ulimit: %ld bytes\n", limit.rlim_cur);
    }

    if (limit.rlim_max == RLIM_INFINITY) {
        printf("Max ulimit: unlimited\n");
    } else {
        printf("Max ulimit: %ld bytes\n", limit.rlim_max);
    }
}

void change_ulimit(char *new_limit) {
    struct rlimit limit;
    long new_value = atol(new_limit);

    if (new_value < 0 && new_value != RLIM_INFINITY) {
        fprintf(stderr, "Invalid ulimit value: %ld\n", new_value);
        return;
    }

    getrlimit(RLIMIT_FSIZE, &limit);
    limit.rlim_cur = new_value;

    if (setrlimit(RLIMIT_FSIZE, &limit) == -1) {
        perror("Failed to set ulimit");
    } else {
        printf("New ulimit set to: %ld bytes\n", new_value);
        print_ulimit();
    }
}

void print_core_size() {
    struct rlimit limit;
    getrlimit(RLIMIT_CORE, &limit);

    if (limit.rlim_cur == RLIM_INFINITY) {
        printf("Core file size: unlimited\n");
    } else {
        printf("Core file size: %ld bytes\n", limit.rlim_cur);
    }
}

void change_core_size(char *new_size) {
    struct rlimit limit;
    long size = atol(new_size);

    if (size < 0 && size != RLIM_INFINITY) {
        fprintf(stderr, "Invalid core file size: %ld\n", size);
        return;
    }

    getrlimit(RLIMIT_CORE, &limit);
    limit.rlim_cur = size;

    if (setrlimit(RLIMIT_CORE, &limit) == -1) {
        perror("Failed to set core file size");
    } else {
        printf("New core file size set to: %ld bytes\n", size);
        print_core_size();
    }
}

void print_working_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("Failed to get current working directory");
    }
}

void print_env_vars() {
    extern char **environ;
    char **env = environ;

    while (*env) {
        printf("%s\n", *env);
        env++;
    }
}

void set_env_var(char *new_var) {
    char *name = strtok(new_var, "=");
    char *value = strtok(NULL, "=");

    if (name && value) {
        if (setenv(name, value, 1) == -1) {
            perror("Failed to set environment variable");
        } else {
            printf("Environment variable %s set to %s\n", name, value);
            print_env_vars();
        }
    } else {
        fprintf(stderr, "Invalid format for environment variable. Use NAME=VALUE\n");
    }
}

int main(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "ispucC:U:dvV:")) != -1) {
        switch (opt) {
            case 'i':
                print_user_info();
                break;
            case 's':
                set_process_group_leader();
                break;
            case 'p':
                print_process_info();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'U':
                change_ulimit(optarg);
                break;
            case 'c':
                print_core_size();
                break;
            case 'C':
                change_core_size(optarg);
                break;
            case 'd':
                print_working_directory();
                break;
            case 'v':
                print_env_vars();
                break;
            case 'V':
                set_env_var(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

