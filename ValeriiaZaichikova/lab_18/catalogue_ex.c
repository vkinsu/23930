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

char getType (struct stat file_s) {
    if (S_ISDIR(file_s.st_mode)) {
        return 'd';
    }
    else if (S_ISREG(file_s.st_mode)) {
        return '-';
    }
    return '?';
}

void getPermissions (struct stat file_s) {
    char permissions[10] = "---------";
    // Owner permissions
    if (file_s.st_mode & S_IRUSR) permissions[0] = 'r';
    if (file_s.st_mode & S_IWUSR) permissions[1] = 'w';
    if (file_s.st_mode & S_IXUSR) permissions[2] = 'x';
    // Group permissions
    if (file_s.st_mode & S_IRGRP) permissions[3] = 'r';
    if (file_s.st_mode & S_IWGRP) permissions[4] = 'w';
    if (file_s.st_mode & S_IXGRP) permissions[5] = 'x';
    // Others permissions
    if (file_s.st_mode & S_IROTH) permissions[6] = 'r';
    if (file_s.st_mode & S_IWOTH) permissions[7] = 'w';
    if (file_s.st_mode & S_IXOTH) permissions[8] = 'x'; 

    printf ("%s ", permissions);
}

void getPasswd (struct stat file_s) {
    struct passwd *pw = getpwuid(file_s.st_uid);

    if (pw)
        printf ("%s ", pw->pw_name);
    else
        printf ("unknown ");
}

void getGroup (struct stat file_s) {
    struct group *gr = getgrgid(file_s.st_gid);

    if (gr)
        printf ("%s ", gr->gr_name);
    else
        printf ("unknown ");
}

void getTime (struct stat file_s) {
    char time_str[20];
    struct tm *timeinfo = localtime(&file_s.st_mtime);

    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", timeinfo);

    printf ("%s ", time_str);
}

void getName (const char* filename) {
    char *name = strrchr(filename, '/');

    if (name) 
        name++;
    else 
        name = (char *)filename; 

    printf ("%s ", name);
}

void getFileInfo(const char *filename) {
    struct stat file_stat;

    if (stat(filename, &file_stat) == -1) {
        perror("Failed to get file info.");
        return;
    }

    char type = getType (file_stat);
    printf ("%c", type);

    getPermissions (file_stat);

    int links = file_stat.st_nlink;
    printf ("\t%d ", links);

    getPasswd (file_stat);
    getGroup (file_stat);

    off_t size = file_stat.st_size;
    printf ("\t%lld ", (long long) size);

    getTime (file_stat);

    getName (filename);
    
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) 
        getFileInfo (".");
    
    for (int i = 1; i < argc; i++) 
        getFileInfo (argv[i]);
    
    return 0;
}
