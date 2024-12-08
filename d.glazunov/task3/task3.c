#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main() {
    // Получение реального и эффективного идентификаторов пользователя
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    // Печать реального и эффективного идентификаторов пользователя
    printf("Current Real UID: %d\n", real_uid);
    printf("Current Effective UID: %d\n", effective_uid);

    // Открытие файла
    FILE *file = fopen("file.txt", "r");

    if (file == NULL) {
        perror("File opening error");
    } else {
        printf("File opened successfully!\n");
        fclose(file);
    }

    // Установка реального идентификатора пользователя в эффективный
    if (setuid(real_uid) == -1) {
        perror("setuid error");
        exit(EXIT_FAILURE);
    }

    printf("Real and Effective UID are now the same.\n");

    // Повторная печать реального и эффективного идентификаторов пользователя
    printf("New Real UID: %d\n", getuid());
    printf("New Effective UID: %d\n", geteuid());

    // Повторное открытие файла
    file = fopen("file.txt", "r");

    if (file == NULL) {
        perror("File opening error");
    } else {
        printf("File opened successfully!\n");
        fclose(file);
    }

    return 0;
}
