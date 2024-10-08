#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

void ids_printing() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t effective_gid = getegid();
    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);
    printf("Real GID: %d, Effective GID: %d\n", real_gid, effective_gid);
}

void process_info_printing() {
    printf("PID: %d, PPID: %d, PGID: %d\n", getpid(), getppid(), getpgrp());
}

void ulimit_printing() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_FSIZE, &limit) == -1) {
        perror("Failed to get ulimit");
    } else {
        if (limit.rlim_cur == RLIM_INFINITY) {
            printf("File size limit (ulimit): unlimited\n");
        } else {
            printf("File size limit (ulimit): %ld bytes\n", limit.rlim_cur);
        }
    }
}

void set_ulimit_printing(char *new_ulimit) {
    char *endptr;
    long value = strtol(new_ulimit, &endptr, 10);
    if (*endptr != '\0' || value < 0) {
        fprintf(stderr, "Invalid ulimit value: %s\n", new_ulimit);
        return;
    }

    struct rlimit limit;
    if (getrlimit(RLIMIT_FSIZE, &limit) == -1) {
        perror("Failed to get current file size limit");
        return;
    }

    limit.rlim_cur = value;
    if (setrlimit(RLIMIT_FSIZE, &limit) == -1) {
        perror("Failed to set file size limit");
    } else {
        printf("New file size limit set to: %ld bytes\n", value);
        ulimit_printing();
    }
}

void core_size_printing() {
    struct rlimit core_limit;
    if (getrlimit(RLIMIT_CORE, &core_limit) == -1) {
        perror("Failed to get core file size limit");
    } else {
        if (core_limit.rlim_cur == RLIM_INFINITY) {
            printf("Core file size: unlimited\n");
        } else {
            printf("Core file size: %ld bytes\n", core_limit.rlim_cur);
        }
    }
}

void set_core_size_printing(char *size) {
    char *endptr;
    long value = strtol(size, &endptr, 10);
    if (*endptr != '\0' || value < 0) {
        fprintf(stderr, "Invalid core file size: %s\n", size);
        return;
    }

    struct rlimit core_limit;
    core_limit.rlim_cur = value;
    core_limit.rlim_max = value;
    if (setrlimit(RLIMIT_CORE, &core_limit) == -1) {
        perror("Failed to set core file size limit");
    } else {
        printf("New core file size limit: %ld bytes\n", value);
        core_size_printing();
    }
}

void working_directory_printing() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

void environment_printing() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        printf("%s\n", *env);
    }
}

void set_environment_variable_printing(char *var) {
    char *name = strtok(var, "=");
    char *value = strtok(NULL, "=");

    if (!name || !value) {
        fprintf(stderr, "Invalid environment variable format: %s\n", var);
        return;
    }

    if (setenv(name, value, 1) == -1) {
        perror("Failed to set environment variable");
    } else {
        printf("Environment variable %s set to %s\n", name, value);
        environment_printing();
    }
}

void process_options(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "ispucC:U:dvV:")) != -1) {
        switch (opt) {
            case 'i':
                ids_printing();
                break;
            case 's':
                if (setpgid(0, 0) == -1) {
                    perror("Failed to set process group leader");
                } else {
                    printf("Process is now the group leader\n");
                }
                break;
            case 'p':
                process_info_printing();
                break;
            case 'u':
                ulimit_printing();
                break;
            case 'U':
                set_ulimit_printing(optarg);
                break;
            case 'c':
                core_size_printing();
                break;
            case 'C':
                set_core_size_printing(optarg);
                break;
            case 'd':
                working_directory_printing();
                break;
            case 'v':
                environment_printing();
                break;
            case 'V':
                set_environment_variable_printing(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    process_options(argc, argv);
    return 0;
}
