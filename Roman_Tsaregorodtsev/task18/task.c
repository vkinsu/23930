#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file_info(const char *filename)
{
    struct stat file_stat;
    if (stat(filename, &file_stat) == -1)
    {
        perror("stat");
        return;
    }

    char file_type = '?';
    if (S_ISDIR(file_stat.st_mode))
        file_type = 'd';
    else if (S_ISREG(file_stat.st_mode))
        file_type = '-';

    char permissions[10];
    permissions[0] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';

    int link_count = file_stat.st_nlink;

    struct passwd *pwd = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);
    const char *owner_name = pwd ? pwd->pw_name : "unknown";
    const char *group_name = grp ? grp->gr_name : "unknown";

    char size_str[40];
    sprintf(size_str, "%ld", file_stat.st_size);
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    const char *file_name = strrchr(filename, '/');
    file_name = file_name ? file_name + 1 : filename;
    printf("%c%s %2d %-8s %-8s %10s %s %s\n", file_type, permissions, link_count, owner_name, group_name, size_str, time_str, file_name);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("too few arguments");
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i < argc; i++)
        print_file_info(argv[i]);
    return 0;
}
