#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

char *get_file_mode(mode_t mode) {
    static char buffer[10];
    buffer[0] = (mode & S_IRUSR) ? 'r' : '-';
    buffer[1] = (mode & S_IWUSR) ? 'w' : '-';
    buffer[2] = (mode & S_IXUSR) ? 'x' : '-';
    buffer[3] = (mode & S_IRGRP) ? 'r' : '-';
    buffer[4] = (mode & S_IWGRP) ? 'w' : '-';
    buffer[5] = (mode & S_IXGRP) ? 'x' : '-';
    buffer[6] = (mode & S_IROTH) ? 'r' : '-';
    buffer[7] = (mode & S_IWOTH) ? 'w' : '-';
    buffer[8] = (mode & S_IXOTH) ? 'x' : '-';
    buffer[9] = '\0';
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Please input the file to scan.");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        struct stat sb;
        if (lstat(argv[i], &sb) == -1) {
            perror(argv[i]);
            return 0;
        }

        char file_type;
        if (S_ISREG(sb.st_mode)) {
            file_type = '-';
        }
        else if (S_ISDIR(sb.st_mode)) {
            file_type = 'd';
        }
        else {
            file_type = '?';
        }

        int link_count = sb.st_nlink;

        struct passwd *pw = getpwuid(sb.st_uid);
        struct group *gr = getgrgid(sb.st_gid);

        char owner[32];
        char group[32];
        snprintf(owner, sizeof(owner), "%s", pw ? pw->pw_name : "?");
        snprintf(group, sizeof(group), "%s", gr ? gr->gr_name : "?");

        char size[16];
        if (file_type == '-') {
            snprintf(size, sizeof(size), "%ld", sb.st_size);
        }
        else {
            snprintf(size, sizeof(size), "");
        }

        char time_buffer[20];
        struct tm *loct = localtime(&sb.st_mtime);
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", loct);

        char *filename = basename((char *)argv[i]);

        printf("%c%s %3d %-8s %-8s %8s %s %s\n",
           file_type, get_file_mode(sb.st_mode), link_count, owner, group, size, time_buffer, filename);
    }

    return 0;
}

