#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

void print_file_info(const char *filename) {
    struct stat file_stat;
    if (stat(filename, &file_stat) == -1) {
        perror("Failed to get file info.");
        return;
    }

    char type = '?';
    if (S_ISDIR(file_stat.st_mode)) {
        type = 'd';
    }
    else if (S_ISREG(file_stat.st_mode)) {
        type = '-';
    }

    char permissions[10] = "---------";
    if (file_stat.st_mode & S_IRUSR) permissions[0] = 'r'; // Owner read
    if (file_stat.st_mode & S_IWUSR) permissions[1] = 'w'; // Owner write
    if (file_stat.st_mode & S_IXUSR) permissions[2] = 'x'; // Owner execute
    if (file_stat.st_mode & S_IRGRP) permissions[3] = 'r'; // Group read
    if (file_stat.st_mode & S_IWGRP) permissions[4] = 'w'; // Group write
    if (file_stat.st_mode & S_IXGRP) permissions[5] = 'x'; // Group execute
    if (file_stat.st_mode & S_IROTH) permissions[6] = 'r'; // Others read
    if (file_stat.st_mode & S_IWOTH) permissions[7] = 'w'; // Others write
    if (file_stat.st_mode & S_IXOTH) permissions[8] = 'x'; // Others execute

    int links = file_stat.st_nlink;

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);

    off_t size = file_stat.st_size;

    char time_str[20];
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", timeinfo);

    char *base_name = strrchr(filename, '/');
    if (base_name) 
        base_name++;
    else 
        base_name = (char *)filename; 
    
    printf("%c%s %2d %-5s %-5s %8lld %s %s\n",
           type, permissions, links,
           pw ? pw->pw_name : "unknown",
           gr ? gr->gr_name : "unknown",
           (long long)size, time_str, base_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) 
        print_file_info(".");
    
    for (int i = 1; i < argc; i++) 
        print_file_info(argv[i]);
    return 0;
}