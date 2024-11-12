#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

void print_file_info(const char *filepath) {
    struct stat file_stat;
    if (lstat(filepath, &file_stat) == -1) {
        perror("lstat error");
        return;
    }

    char file_type;
    if (S_ISREG(file_stat.st_mode)) {
        file_type = '-';
    } else if (S_ISDIR(file_stat.st_mode)) {
        file_type = 'd';
    } else {
        file_type = '?';
    }

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

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    char owner[32];
    char group[32];
    snprintf(owner, sizeof(owner), "%s", pw ? pw->pw_name : "UNKNOWN");
    snprintf(group, sizeof(group), "%s", gr ? gr->gr_name : "UNKNOWN");

    char size[16];
    if (file_type == '-') {
        snprintf(size, sizeof(size), "%ld", file_stat.st_size);
    } else {
        snprintf(size, sizeof(size), ""); // Оставляем пустым для каталогов
    }

    char mod_time[20];
    struct tm *mtm = localtime(&file_stat.st_mtime);
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M", mtm);

    char *base_name = basename((char *)filepath);

    printf("%c%s %3d %-8s %-8s %8s %s %s\n",
           file_type, permissions, link_count, owner, group, size, mod_time, base_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}
