#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 256

int main(int argc, char *argv[]) {
    long lines_pos[500];// Массив для позиций строк
    int line_ln[500];
    int fd;
    int i = 1, j = 0, line_number;
    char c, buf[MAX_LINE_LENGTH + 1];

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    // Открытие файла
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        printf("Failed to open file.\n");
        exit(1);
    }

    lines_pos[1] = 0L; // Первая строка начинается с нулевого отступа

    // Считывание файла для построения таблицы строк
    while (read(fd, &c, 1)) {
        if (c == '\n') {
            line_ln[i] = j + 1;  // Длина строки с учётом '\n'
            lines_pos[++i] = lseek(fd, 0L, SEEK_CUR);  // Сохраняем смещение начала новой строки
            j = 0;  // Обнуляем счетчик символов для новой строки
        } else {
            j++;  // Увеличиваем длину текущей строки
        }
    }

    // Основной цикл запроса номера строки
    while (1) {
        printf("Enter line number (0 to exit): ");
        scanf("%d", &line_number);

        if (line_number == 0) {
            close(fd);
            exit(0);
        }

        if (line_number < 1 || line_number >= i) {
            printf("Invalid line number\n");
            continue;
        }

        // Перемещение на начало нужной строки
        lseek(fd, lines_pos[line_number], SEEK_SET);

        // Чтение строки и вывод
        int bytes_read = read(fd, buf, line_ln[line_number]);
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';  // Завершаем строку нулевым символом
            printf("%s", buf);  // Печатаем строку
        } else {
            printf("Error reading line\n");
        }
    }

    close(fd);

    return 0;
}
