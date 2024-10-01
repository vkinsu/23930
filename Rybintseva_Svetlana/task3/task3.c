#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void print_user_id(){
    uid_t uid = getuid();
    uid_t euid = geteuid();
    printf("%d\n", uid);
    printf("%d\n", euid);
}

void open_file(){
    FILE *file;
    file = fopen("data.txt", "r");
    if (file == NULL){
        perror("error: ");
        return;
    }
    fclose(file);
}

int main(){
    print_user_id();
    open_file();
    uid_t uid = getuid();
    setuid(uid);
    print_user_id();
    open_file();
    return 0;
}