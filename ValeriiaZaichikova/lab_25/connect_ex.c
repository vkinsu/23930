#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main() {
    int fildes[2];
    if (pipe(fildes) == -1) { return 1; }

    pid_t pid = fork();
    if (pid == -1) { return 1; }

    if (pid == 0) {
        close(fildes[1]);

        char c;
        while (read(fildes[0], &c, 1) != 0) {
            printf("%c", toupper(c));
        }

        close(fildes[0]);
    } else {
        close(fildes[0]);

        char *text = "Hello, World!\0";
        for (char *ptr = text; *ptr != 0; ptr++) {
            write(fildes[1], ptr, 1);
        }

        close(fildes[1]);
        wait(NULL);
    }

    return 0;
}