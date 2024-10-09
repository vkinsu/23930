#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

extern char **environ;

int main(int argc, char *argv[]) {
    char options[] = ":ispuU:cC:dvV:";  
    int c;
    struct rlimit limits;
    
    while ((c = getopt(argc, argv, options)) != EOF) {
        switch (c) {
            case 'i': // -i  Печатает реальные и эффективные идентификаторы пользователя и группы.
                printf("\nuid: %d, euid: %d, gid: %d, egid: %d\n", getuid(), geteuid(), getgid(), getegid());
                break;

            case 's': // -s  Процесс становится лидером группы. Подсказка: смотри setpgid(2).
                if (-1 == setpgid(0, 0))
                    perror("\nfailed to set the group leader process\n");
                else
                    printf("\nsuccess: process set as group leader\n");
                break;

            case 'p': // -p  Печатает идентификаторы процесса, процесса-родителя и группы процессов.
                printf("\npid: %d, ppid: %d, pgrp: %d\n", getpid(), getppid(), getpgrp());
                break;

            case 'u': // -u  Печатает значение ulimit.
                if (-1 == getrlimit(RLIMIT_FSIZE, &limits))
                    perror("\nfailed to get ulimit\n");
                else
                    printf("\nulimit = %lu\n", limits.rlim_max);
                break;

            case 'U': // -Unew_ulimit  Изменяет значение ulimit. Подсказка: смотри atol(3C) на странице руководства strtol(3C).
                long new_ulimit = strtol(optarg, NULL, 10);
                if (0 == new_ulimit){
                    perror("\ninvalid argument for the -U option\n");
                    break;
                }
                struct rlimit limits; // from case 'u'
                if (-1 == getrlimit(RLIMIT_FSIZE, &limits)) {
                    perror("\nfailed to get the ulimit\n");
                    break;
                }
                limits.rlim_cur = new_ulimit;
                if (-1 == setrlimit(RLIMIT_FSIZE, &limits))
                    perror("\nfailed to set the ulimit\n");
                else
                    printf("\nsuccess: the ulimit has been set\n");
                break;

            case 'c': // -c  Печатает размер в байтах core-файла, который может быть создан.
                if (-1 == getrlimit(RLIMIT_CORE, &limits)) {
                    perror("\nfailed to get the core-file size limit\n");
                    break;
                } else 
                    printf("\ncore-file size limit = %lu\n", limits.rlim_max);
                break;

            case 'C': // -Csize  Изменяет размер core-файла.
                long long new_limit = strtoll(optarg, NULL, 10);
                if (0 == new_limit) {
                    perror("\ninvalid argument for the -C option\n");
                    break;
                }
                if (-1 == getrlimit(RLIMIT_CORE, &limits)) {
                    perror("\nfailed to get the core-file size limit\n");
                    break;
                }
                limits.rlim_cur = new_limit;
                if (-1 == setrlimit(RLIMIT_CORE, &limits))
                    perror("\nfailed to set the core-file size limit\n");
                else
                    printf("\nsuccess: the core-file size limit has been set\n");
                break;

            case 'd': // -d  Печатает текущую рабочую директорию.
                char *pathname = getenv("PWD");
                if (NULL == pathname)
                    perror("\nfailed to get the current directory\n");
                else
                    printf("\ncurrent directory: %s\n", pathname);
                break;

            case 'v': // -v  Распечатывает переменные среды и их значения.
                char **ptr = environ;
                printf("\n");
                for (; *ptr != NULL; ptr++)
                    printf("%s\n", *ptr);
                break;

            case 'V': // -Vname=value  Вносит новую переменную в среду или изменяет значение существующей переменной.
                if (-1 == putenv(optarg))
                    perror("\nfailed to set the environironmental variable");
                break;
            
            case ':':
                printf("\nmissing argument for option: %c\n", optopt);
                break;

            default:
                printf("\ninvalid option: %c\n", optopt);
        }
    }
    
    return 0;
}