#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void print_user_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    printf("%d\n", real_uid);
    printf("%d\n", effective_uid);
}

void open_and_close_file() {
    char *filename = "example.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("error");
    } else {
        printf("File opened successfully\n");
        fclose(file);
    }
}

int main() {
    print_user_ids();
    open_and_close_file();
    setuid(getuid());
    print_user_ids();
    open_and_close_file();
    return 0;
}
