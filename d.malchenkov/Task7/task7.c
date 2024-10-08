#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 256

int fd;
char *file_memory;
size_t file_size;

void handle_timeout(int sig) {
    printf("\nTime is up! Printing the entire file:\n");
    // Печатаем содержимое файла, отображенного в память
    write(1, file_memory, file_size);
    exit(0);
}

int main(int argc, char *argv[]) {
    long lines_pos[500];  // Массив для позиций строк
    int line_ln[500];
    int i = 1, j = 0, line_number;
    char c;
    struct stat st;

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    // Открытие файла
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Failed to open file");
        exit(1);
    }

    // Получаем информацию о файле для отображения в память
    if (fstat(fd, &st) == -1) {
        perror("Failed to get file size");
        close(fd);
        exit(1);
    }
    file_size = st.st_size;

    // Отображение файла в память
    file_memory = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_memory == MAP_FAILED) {
        perror("Failed to map file");
        close(fd);
        exit(1);
    }

    signal(SIGALRM, handle_timeout);

    lines_pos[1] = 0L; // Первая строка начинается с нулевого отступа

    // Считывание файла для построения таблицы строк
    for (size_t k = 0; k < file_size; k++) {
        c = file_memory[k];
        if (c == '\n') {
            line_ln[i] = j + 1;  // Длина строки с учётом '\n'
            lines_pos[++i] = k + 1;  // Сохраняем смещение начала новой строки
            j = 0;  // Обнуляем счетчик символов для новой строки
        } else {
            j++;  // Увеличиваем длину текущей строки
        }
    }

    // Основной цикл запроса номера строки
    while (1) {
        alarm(5);
        printf("Enter line number (0 to exit): ");
        scanf("%d", &line_number);
        alarm(0);

        if (line_number == 0) {
            break;  // Выход из программы
        }

        if (line_number < 1 || line_number >= i) {
            printf("Invalid line number\n");
            continue;
        }

        // Вывод строки с использованием указателя на память
        printf("%.*s", line_ln[line_number], file_memory + lines_pos[line_number]);
    }

    // Освобождение памяти и закрытие файла
    munmap(file_memory, file_size);
    close(fd);

    return 0;
}
