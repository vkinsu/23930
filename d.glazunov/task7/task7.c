#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>

int fd;
char *file_content;
size_t file_size;

void timeout_handler(int sig) {
    printf("\nTime is up!\n\n");
    write(1, file_content, file_size); // Выводим весь файл на экран
    munmap(file_content, file_size);   // Освобождаем память
    exit(0); // Завершаем программу
}

int main(int argc, char *argv[]) {
    long lines_pos[500];
    int i = 1, j = 0, line_number, line_ln[500];
    char *buf;
    struct stat st;

    // Открытие файла
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Failed to open file");
        exit(1);
    }

    // Получаем размер файла
    if (fstat(fd, &st) == -1) {
        perror("Failed to get file size");
        exit(1);
    }
    file_size = st.st_size;

    // Отображаем файл в память
    file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("Failed to map file");
        exit(1);
    }

    // Определение позиций строк
    lines_pos[1] = 0L; // Первая строка начинается с начала файла
    for (size_t k = 0; k < file_size; k++) {
        if (file_content[k] == '\n') {
            j++;
            line_ln[i++] = j;
            lines_pos[i] = k + 1; // Сохраняем позицию следующей строки

            // Выводим информацию о строке для тестирования
            printf("%ld  %.*s  %d\n", lines_pos[i - 1], j, &file_content[lines_pos[i - 1]], j);
            j = 0;
        } else {
            j++;
        }
    }

    // Установка обработчика сигнала для таймера
    signal(SIGALRM, timeout_handler);

    // Основной цикл программы
    while (1) {
        printf("Enter line number. You have 5 seconds: ");
        alarm(5); // Устанавливаем таймер на 5 секунд

        if (scanf("%d", &line_number) == 1) {
            alarm(0); // Сбрасываем таймер при успешном вводе

            if (line_number == 0)
                break; // Завершение программы при вводе 0

            if (line_number > 0 && line_number < i) {
                // Выводим строку
                printf("%.*s", line_ln[line_number], &file_content[lines_pos[line_number]]);
            } else {
                printf("Invalid line number\n");
            }
        } else {
            alarm(0); // Сбрасываем таймер при ошибке ввода
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Очищаем буфер ввода
        }
    }

    // Освобождение отображенной памяти
    munmap(file_content, file_size);
    close(fd);

    return 0;
}
