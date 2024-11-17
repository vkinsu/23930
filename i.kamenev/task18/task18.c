#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file_info(const char *path) {
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1) {
        perror("lstat");
        return;
    }

    // Определяем тип файла
    char file_type;
    if (S_ISDIR(file_stat.st_mode)) file_type = 'd';
    else if (S_ISREG(file_stat.st_mode)) file_type = '-';
    else file_type = '?';

    // Определяем права доступа
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

    // Количество связей
    int link_count = file_stat.st_nlink;

    // Имя собственника и группы
    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    char *owner = pw ? pw->pw_name : "UNKNOWN";
    char *group = gr ? gr->gr_name : "UNKNOWN";

    // Размер файла
    char size[16];
    if (S_ISREG(file_stat.st_mode))
        snprintf(size, sizeof(size), "%lld", (long long)file_stat.st_size);
    else
        size[0] = '\0';

    // Дата модификации
    char mod_time[20];
    struct tm *mtm = localtime(&file_stat.st_mtime);
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M", mtm);

    // Имя файла
    const char *file_name = strrchr(path, '/');
    file_name = file_name ? file_name + 1 : path;

    // Вывод информации
    printf("%c%s %2d %-8s %-8s %8s %s %s\n",
           file_type, permissions, link_count, owner, group, size, mod_time, file_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Type Perms    Links Owner    Group    Size     Modified          Name\n");
    printf("--------------------------------------------------------------------------\n");
    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return EXIT_SUCCESS;
}

