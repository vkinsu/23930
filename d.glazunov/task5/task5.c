#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    long lines_pos[500]; // Массив для хранения позиций строк
    int line_lengths[500]; // Массив для хранения длин строк
    int fd, i = 1, char_count = 0, line_number;
    char c, buf[257];

    // Открываем файл для чтения
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        printf("Failed to open the file.\n");
        exit(1);
    }

    // Первая строка начинается с отступа 0
    lines_pos[1] = 0L;

    // Проходим по файлу и определяем позиции строк
    while (read(fd, &c, 1) > 0) {
        if (c == '\n') {
            line_lengths[i] = char_count; // Запоминаем длину строки
            lines_pos[++i] = lseek(fd, 0L, SEEK_CUR); // Позиция начала следующей строки
            char_count = 0; // Сброс счетчика символов
        } else {
            char_count++;
        }
    }

    // Запрашиваем номер строки и выводим соответствующую строку
    while (1) {
        printf("Enter the line number (0 to exit): ");
        scanf("%d", &line_number);

        if (line_number == 0) {
            close(fd); // Закрываем файл перед выходом
            exit(0);
        }

        if (line_number < 1 || line_number >= i) {
            printf("Invalid line number\n");
            continue;
        }

        // Позиционируемся на начало нужной строки
        lseek(fd, lines_pos[line_number], SEEK_SET);

        // Читаем строку нужной длины и выводим её
        if (read(fd, buf, line_lengths[line_number]) > 0) {
            buf[line_lengths[line_number]] = '\0'; // Добавляем символ конца строки
            printf("%s\n", buf);
        } else {
            printf("Error reading the line\n");
        }
    }

    return 0;
}
