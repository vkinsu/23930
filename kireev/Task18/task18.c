#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void print_permissions(mode_t mode) {
    char file_type;
    if (S_ISDIR(mode)) file_type = 'd';
    else if (S_ISREG(mode)) file_type = '-';
    else file_type = '?';

    printf("%c", file_type);
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
}

void print_file_info(const char *path) {
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1) {
        perror(path);
        return;
    }

    print_permissions(file_stat.st_mode);
    printf(" ");

    printf("%ld ", (long)file_stat.st_nlink);

    struct passwd *pwd = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);
    printf("%s %s ", pwd ? pwd->pw_name : "?", grp ? grp->gr_name : "?");

    if (S_ISREG(file_stat.st_mode)) {
        printf("%ld ", (long)file_stat.st_size);
    }

    char time_buf[20];
    struct tm *time_info = localtime(&file_stat.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", time_info);
    printf("%s ", time_buf);

    char *base_name = strrchr(path, '/');
    printf("%s\n", base_name ? base_name + 1 : path);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    exit(0);
}
