#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(0);
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    // Захватываем файл с помощью fcntl
    struct flock fl;
    fl.l_type = F_WRLCK;  // Блокировка на запись
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;  // Захватываем весь файл

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        if ((errno == EACCES) || (errno == EAGAIN)) {
            printf("File %s locked, try again later\n", argv[1]);
            exit(2);
        }
        perror("fcntl");
        close(fd);
        return 1;
    }

    printf("File %s locked. Starting editor...\n", argv[1]);

    // Запускаем редактор через system
    char command[256];
    snprintf(command, sizeof(command), "nano %s", argv[1]);
    system(command);

    // Освобождаем файл после завершения работы
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);

    printf("File %s unlocked.\n", argv[1]);
    close(fd);
    return 0;
}

/*
Когда файл захвачен с использованием обязательного механизма, другие процессы не могут читать/изменять файл, до тех пор, пока файл заблокирован.
Это блокирует любые операции с файлом. Если кто либо попытается открыть/прочитать файл, процесс либо будет заблокирован до освобождения файла, либо получит ошибку.
*/
