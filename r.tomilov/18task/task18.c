#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void print_file_info(const char *path) {
    struct stat file_stat;
    struct passwd *pwd;
    struct group *grp;
    char date[20];
    char permissions[11] = "----------";
    char *filename;

    if (stat(path, &file_stat) == -1) {
        perror("stat");
        return;
    }

    // Определение типа файла
    if (S_ISDIR(file_stat.st_mode)) permissions[0] = 'd';
    else if (S_ISREG(file_stat.st_mode)) permissions[0] = '-';
    else permissions[0] = '?';

    // Права доступа
    permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';

    permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';

    permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';

    // Количество ссылок
    int links = file_stat.st_nlink;

    // Имя владельца и группы
    pwd = getpwuid(file_stat.st_uid);
    char *owner = pwd ? pwd->pw_name : "unknown";

    grp = getgrgid(file_stat.st_gid);
    char *group = grp ? grp->gr_name : "unknown";

    // Размер файла
    off_t size = file_stat.st_size;

    // Дата последней модификации
    struct tm *tm = localtime(&file_stat.st_mtime);
    strftime(date, sizeof(date), "%b %e %H:%M", tm);

    // Имя файла из пути
    filename = strrchr(path, '/');
    filename = filename ? filename + 1 : (char *)path;

    // Вывод информации
    printf("%s %3d %-8s %-8s %8ld %s %s\n",
           permissions, links, owner, group, size, date, filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // Если не указаны аргументы, по умолчанию проверяем текущий каталог
        print_file_info(".");
    } else {
        for (int i = 1; i < argc; i++) {
            print_file_info(argv[i]);
        }
    }
    return 0;
}
