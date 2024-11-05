#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>


void print_info(const char* filename) {
  struct stat file_info;
  if (stat(filename, &file_info) == -1) {
    printf("\nCan not get %s info\n", filename);
    return;
  }
  off_t size = 0;
  char file_type = '?';
  if (S_ISDIR(file_info.st_mode)) {
    file_type = 'd';
  } else if (S_ISREG(file_info.st_mode)) {
    file_type = '-';
    size = file_info.st_size;
  }

  char permissions[10] = "---------\0";
  if (file_info.st_mode & S_IRUSR) permissions[0] = 'r';
  if (file_info.st_mode & S_IWUSR) permissions[1] = 'w';
  if (file_info.st_mode & S_IXUSR) permissions[2] = 'x';
  if (file_info.st_mode & S_IRGRP) permissions[3] = 'r';
  if (file_info.st_mode & S_IWGRP) permissions[4] = 'w';
  if (file_info.st_mode & S_IXGRP) permissions[5] = 'x';
  if (file_info.st_mode & S_IROTH) permissions[6] = 'r';
  if (file_info.st_mode & S_IWOTH) permissions[7] = 'w';
  if (file_info.st_mode & S_IXOTH) permissions[8] = 'x';

  int links = file_info.st_nlink;

  struct passwd* pwd = getpwuid(file_info.st_uid);
  struct group* grp = getgrgid(file_info.st_gid);
  const char* owner = pwd ? pwd->pw_name : "unknown";
  const char* group = grp ? grp->gr_name : "unknown";
  struct tm *time_info = localtime(&file_info.st_mtime);
  char time_str[40];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
  char *file_finding_path = strrchr(filename, '/');
  const char* file_final_path = (file_finding_path) ? file_finding_path + 1 : filename;

  if (size == 0) {
    printf("%c%s %2d %10s %10s %9s %s %s\n", file_type, permissions, links, owner, group, "\0", time_str, file_final_path);
  } else {
    printf("%c%s %2d %10s %10s %9ld %s %s\n", file_type, permissions, links, owner, group, size, time_str, file_final_path);
  }
}


int main(int argc, const char* argv[]) {
  if (argc < 2) {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    print_info(cwd);
    return 1;
  }

  for (int i = 1; i < argc; ++i) {
    print_info(argv[i]);
  }

  return 0;
}