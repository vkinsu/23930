#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void fileinfo(const char *path) {
    struct stat fileStat;
    if (lstat(path, &fileStat) == -1) {
        perror("no file info");
        return;
    }
    char type;
    switch (fileStat.st_mode & S_IFMT) {
        case S_IFDIR:
            type = 'd';
            break;
        case S_IFREG:
            type = '-';
            break;
        default:
            type = '?';
            break;
    }
    char permissions[10];
    permissions[0] = (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (fileStat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (fileStat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (fileStat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (fileStat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';
    int numLinks = fileStat.st_nlink;
    struct passwd *pw = getpwuid(fileStat.st_uid);
    const char *owner = pw ? pw->pw_name : "unknown";
    struct group *gr = getgrgid(fileStat.st_gid);
    const char *group = gr ? gr->gr_name : "unknown";
    char size[20] = "";
    if (S_ISREG(fileStat.st_mode)) sprintf(size, "%lld", fileStat.st_size);
    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", localtime(&fileStat.st_mtime));
    const char *fileName = strrchr(path, '/');
    fileName = fileName ? fileName + 1 : path;
    printf("%c%s %3d %-8s %-8s %8s %s %s\n", type, permissions, numLinks, owner, group, size, timeBuffer, fileName);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file/dir...\n", argv[0]);
        return 1;
    }
    for (int i = 1; i < argc; ++i) { fileinfo(argv[i]);}
    return 0;
}

